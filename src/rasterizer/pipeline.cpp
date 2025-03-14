// clang-format off
#include "pipeline.h"

#include <iostream>
#include <unordered_set>
#include <utility>

#include "../lib/log.h"
#include "../lib/mathlib.h"
#include "framebuffer.h"
#include "sample_pattern.h"
template<PrimitiveType primitive_type, class Program, uint32_t flags>
void Pipeline<primitive_type, Program, flags>::run(std::vector<Vertex> const& vertices,
                                                   typename Program::Parameters const& parameters,
                                                   Framebuffer* framebuffer_) {
	// Framebuffer must be non-null:
	assert(framebuffer_);
	auto& framebuffer = *framebuffer_;

	// A1T7: sample loop
	// TODO: update this function to rasterize to *all* sample locations in the framebuffer.
	//  	 This will probably involve inserting a loop of the form:
	// 		 	std::vector< Vec3 > const &samples = framebuffer.sample_pattern.centers_and_weights;
	//      	for (uint32_t s = 0; s < samples.size(); ++s) { ... }
	//   	 around some subset of the code.
	// 		 You will also need to transform the input and output of the rasterize_* functions to
	// 	     account for the fact they deal with pixels centered at (0.5,0.5).

	std::vector<ShadedVertex> shaded_vertices;
	shaded_vertices.reserve(vertices.size());

	//--------------------------
	// shade vertices:
	for (auto const& v : vertices) {
		ShadedVertex sv;
		Program::shade_vertex(parameters, v.attributes, &sv.clip_position, &sv.attributes);
		shaded_vertices.emplace_back(sv);
	}

	//--------------------------
	// assemble + clip + homogeneous divide vertices:
	std::vector<ClippedVertex> clipped_vertices;

	// reserve some space to avoid reallocations later:
	if constexpr (primitive_type == PrimitiveType::Lines) {
		// clipping lines can never produce more than one vertex per input vertex:
		clipped_vertices.reserve(shaded_vertices.size());
	} else if constexpr (primitive_type == PrimitiveType::Triangles) {
		// clipping triangles can produce up to 8 vertices per input vertex:
		clipped_vertices.reserve(shaded_vertices.size() * 8);
	}
	// clang-format off

	//coefficients to map from clip coordinates to framebuffer (i.e., "viewport") coordinates:
	//x: [-1,1] -> [0,width]
	//y: [-1,1] -> [0,height]
	//z: [-1,1] -> [0,1] (OpenGL-style depth range)
	Vec3 const clip_to_fb_scale = Vec3{
		framebuffer.width / 2.0f,
		framebuffer.height / 2.0f,
		0.5f
	};
	Vec3 const clip_to_fb_offset = Vec3{
		0.5f * framebuffer.width,
		0.5f * framebuffer.height,
		0.5f
	};

	// helper used to put output of clipping functions into clipped_vertices:
	auto emit_vertex = [&](ShadedVertex const& sv) {
		ClippedVertex cv;
		float inv_w = 1.0f / sv.clip_position.w;
		cv.fb_position = clip_to_fb_scale * inv_w * sv.clip_position.xyz() + clip_to_fb_offset;
		cv.inv_w = inv_w;
		cv.attributes = sv.attributes;
		clipped_vertices.emplace_back(cv);
	};

	// actually do clipping:
	if constexpr (primitive_type == PrimitiveType::Lines) {
		for (uint32_t i = 0; i + 1 < shaded_vertices.size(); i += 2) {
			clip_line(shaded_vertices[i], shaded_vertices[i + 1], emit_vertex);
		}
	} else if constexpr (primitive_type == PrimitiveType::Triangles) {
		for (uint32_t i = 0; i + 2 < shaded_vertices.size(); i += 3) {
			clip_triangle(shaded_vertices[i], shaded_vertices[i + 1], shaded_vertices[i + 2], emit_vertex);
		}
	} else {
		static_assert(primitive_type == PrimitiveType::Lines, "Unsupported primitive type.");
	}

	//--------------------------
	// rasterize primitives:

	std::vector<Fragment> fragments;

	// helper used to put output of rasterization functions into fragments:
	auto emit_fragment = [&](Fragment const& f) { fragments.emplace_back(f); };

	// actually do rasterization:
	std::vector<Vec3> const &samples = framebuffer.sample_pattern.centers_and_weights;
	auto sample_ = samples;
	if constexpr (primitive_type == PrimitiveType::Lines) {
		for (uint32_t i = 0; i + 1 < clipped_vertices.size(); i += 2) {
			for (uint32_t s = 0; s < samples.size(); ++s) {
				float dx = samples[s].x;
				float dy = samples[s].y;
				rasterize_line(clipped_vertices[i], clipped_vertices[i + 1], emit_fragment, dx, dy);
			}
		}
	} else if constexpr (primitive_type == PrimitiveType::Triangles) {
		for (uint32_t i = 0; i + 2 < clipped_vertices.size(); i += 3) {
			rasterize_triangle(clipped_vertices[i], clipped_vertices[i + 1], clipped_vertices[i + 2], emit_fragment, sample_);
		}
	} else {
		static_assert(primitive_type == PrimitiveType::Lines, "Unsupported primitive type.");
	}

	//--------------------------
	// depth test + shade + blend fragments:
	uint32_t out_of_range = 0; // check if rasterization produced fragments outside framebuffer 
							   // (indicates something is wrong with clipping)
	assert(fragments.size() % samples.size() == 0);
	for (size_t i = 0; i < fragments.size() / samples.size(); i++) {
		for (uint32_t s = 0; s < samples.size(); s++) {

			auto const& f = fragments[i * samples.size() + s];

			// fragment location (in pixels):
			int32_t x = (int32_t)std::floor(f.fb_position.x);
			int32_t y = (int32_t)std::floor(f.fb_position.y);
	
			// if clipping is working properly, this condition shouldn't be needed;
			// however, it prevents crashes while you are working on your clipping functions,
			// so we suggest leaving it in place:
			if (x < 0 || (uint32_t)x >= framebuffer.width || 
				y < 0 || (uint32_t)y >= framebuffer.height) {
				++out_of_range;
				continue;
			}
			// local names that refer to destination sample in framebuffer:
			float& fb_depth = framebuffer.depth_at(x, y, s);
			Spectrum& fb_color = framebuffer.color_at(x, y, s);
			// depth test:
			if constexpr ((flags & PipelineMask_Depth) == Pipeline_Depth_Always) {
				// "Always" means the depth test always passes.
			} else if constexpr ((flags & PipelineMask_Depth) == Pipeline_Depth_Never) {
				// "Never" means the depth test never passes.
				continue; //discard this fragment
			} else if constexpr ((flags & PipelineMask_Depth) == Pipeline_Depth_Less) {
				// "Less" means the depth test passes when the new fragment has depth less than the stored depth.
				// A1T4: Depth_Less
				// TODO: implement depth test! We want to only emit fragments that have a depth less than the stored depth, hence "Depth_Less".
				if (f.fb_position.z >= fb_depth) {
					continue;
				}
			} else {
				static_assert((flags & PipelineMask_Depth) <= Pipeline_Depth_Always, "Unknown depth test flag.");
			}

			// if depth test passes, and depth writes aren't disabled, write depth to depth buffer:
			if constexpr (!(flags & Pipeline_DepthWriteDisableBit)) {
				fb_depth = f.fb_position.z;
			}

			// shade fragment:
			ShadedFragment sf;
			sf.fb_position = f.fb_position;
			Program::shade_fragment(parameters, f.attributes, f.derivatives, &sf.color, &sf.opacity);

			// write color to framebuffer if color writes aren't disabled:
			if constexpr (!(flags & Pipeline_ColorWriteDisableBit)) {
				// blend fragment:
				if constexpr ((flags & PipelineMask_Blend) == Pipeline_Blend_Replace) {
					fb_color = sf.color;
				} else if constexpr ((flags & PipelineMask_Blend) == Pipeline_Blend_Add) {
					// A1T4: Blend_Add
					// TODO: framebuffer color should have fragment color multiplied by fragment opacity added to it.
					fb_color += sf.color * sf.opacity;
				} else if constexpr ((flags & PipelineMask_Blend) == Pipeline_Blend_Over) {
					// A1T4: Blend_Over
					// TODO: set framebuffer color to the result of "over" blending (also called "alpha blending") the fragment color over the framebuffer color, using the fragment's opacity
					// 		 You may assume that the framebuffer color has its alpha premultiplied already, and you just want to compute the resulting composite color
					fb_color = sf.color * sf.opacity + (1 - sf.opacity) * fb_color; 
				} else {
					static_assert((flags & PipelineMask_Blend) <= Pipeline_Blend_Over, "Unknown blending flag.");
				}
			}
		}
	}

	if (out_of_range > 0) {
		if constexpr (primitive_type == PrimitiveType::Lines) {
			warn("Produced %d fragments outside framebuffer; this indicates something is likely "
			     "wrong with the clip_line function.",
			     out_of_range);
		} else if constexpr (primitive_type == PrimitiveType::Triangles) {
			warn("Produced %d fragments outside framebuffer; this indicates something is likely "
			     "wrong with the clip_triangle function.",
			     out_of_range);
		}
	}
}

// -------------------------------------------------------------------------
// clipping functions

// helper to interpolate between vertices:
template<PrimitiveType p, class P, uint32_t F>
auto Pipeline<p, P, F>::lerp(ShadedVertex const& a, ShadedVertex const& b, float t) -> ShadedVertex {
	ShadedVertex ret;
	ret.clip_position = (b.clip_position - a.clip_position) * t + a.clip_position;
	for (uint32_t i = 0; i < ret.attributes.size(); ++i) {
		ret.attributes[i] = (b.attributes[i] - a.attributes[i]) * t + a.attributes[i];
	}
	return ret;
}

/*
 * clip_line - clip line to portion with -w <= x,y,z <= w, emit vertices of clipped line (if non-empty)
 *  	va, vb: endpoints of line
 *  	emit_vertex: call to produce truncated line
 *
 * If clipping shortens the line, attributes of the shortened line should respect the pipeline's interpolation mode.
 * 
 * If no portion of the line remains after clipping, emit_vertex will not be called.
 *
 * The clipped line should have the same direction as the full line.
 */
template<PrimitiveType p, class P, uint32_t flags>
void Pipeline<p, P, flags>::clip_line(ShadedVertex const& va, ShadedVertex const& vb,
                                      std::function<void(ShadedVertex const&)> const& emit_vertex) {
	// Determine portion of line over which:
	// 		pt = (b-a) * t + a
	//  	-pt.w <= pt.x <= pt.w
	//  	-pt.w <= pt.y <= pt.w
	//  	-pt.w <= pt.z <= pt.w
	// ... as a range [min_t, max_t]:

	float min_t = 0.0f;
	float max_t = 1.0f;

	// want to set range of t for a bunch of equations like:
	//    a.x + t * ba.x <= a.w + t * ba.w
	// so here's a helper:
	auto clip_range = [&min_t, &max_t](float l, float dl, float r, float dr) {
		// restrict range such that:
		// l + t * dl <= r + t * dr
		// re-arranging:
		//  l - r <= t * (dr - dl)
		if (dr == dl) {
			// want: l - r <= 0
			if (l - r > 0.0f) {
				// works for none of range, so make range empty:
				min_t = 1.0f;
				max_t = 0.0f;
			}
		} else if (dr > dl) {
			// since dr - dl is positive:
			// want: (l - r) / (dr - dl) <= t
			min_t = std::max(min_t, (l - r) / (dr - dl));
		} else { // dr < dl
			// since dr - dl is negative:
			// want: (l - r) / (dr - dl) >= t
			max_t = std::min(max_t, (l - r) / (dr - dl));
		}
	};

	// local names for clip positions and their difference:
	Vec4 const& a = va.clip_position;
	Vec4 const& b = vb.clip_position;
	Vec4 const ba = b - a;

	// -a.w - t * ba.w <= a.x + t * ba.x <= a.w + t * ba.w
	clip_range(-a.w, -ba.w, a.x, ba.x);
	clip_range(a.x, ba.x, a.w, ba.w);
	// -a.w - t * ba.w <= a.y + t * ba.y <= a.w + t * ba.w
	clip_range(-a.w, -ba.w, a.y, ba.y);
	clip_range(a.y, ba.y, a.w, ba.w);
	// -a.w - t * ba.w <= a.z + t * ba.z <= a.w + t * ba.w
	clip_range(-a.w, -ba.w, a.z, ba.z);
	clip_range(a.z, ba.z, a.w, ba.w);

	if (min_t < max_t) {
		if (min_t == 0.0f) {
			emit_vertex(va);
		} else {
			ShadedVertex out = lerp(va, vb, min_t);
			// don't interpolate attributes if in flat shading mode:
			if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Flat) {
				out.attributes = va.attributes;
			}
			emit_vertex(out);
		}
		if (max_t == 1.0f) {
			emit_vertex(vb);
		} else {
			ShadedVertex out = lerp(va, vb, max_t);
			// don't interpolate attributes if in flat shading mode:
			if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Flat) {
				out.attributes = va.attributes;
			}
			emit_vertex(out);
		}
	}
}

/*
 * clip_triangle - clip triangle to portion with -w <= x,y,z <= w, emit resulting shape as triangles (if non-empty)
 *  	va, vb, vc: vertices of triangle
 *  	emit_vertex: call to produce clipped triangles (three calls per triangle)
 *
 * If clipping truncates the triangle, attributes of the new vertices should respect the pipeline's interpolation mode.
 * 
 * If no portion of the triangle remains after clipping, emit_vertex will not be called.
 *
 * The clipped triangle(s) should have the same winding order as the full triangle.
 */
template<PrimitiveType p, class P, uint32_t flags>
void Pipeline<p, P, flags>::clip_triangle(
	ShadedVertex const& va, ShadedVertex const& vb, ShadedVertex const& vc,
	std::function<void(ShadedVertex const&)> const& emit_vertex) {
	// A1EC: clip_triangle
	// TODO: correct code!
	// emit_vertex(va);
	// emit_vertex(vb);
	// emit_vertex(vc);

	float min_t = 0.0f;
	float max_t = 1.0f;

	auto clip_range = [&min_t, &max_t](float l, float dl, float r, float dr) {
		/**
		 * p(t) = (b - a) * t + a
		 * x(t) = (b.x - a.x) * t + a.x
		 * w(t) = (b.w - a.w) * t + a.w
		 * -w(t) <= x(t) <= w(t)
		 * l + t * dl <= r + t * dr
		 */
		if (dr == dl) {
			if (l - r > 0.0f) {
				min_t = 1.0f;
				max_t = 0.0f;
			}
		} else if (dr > dl) {
			min_t = std::max(min_t, (l - r) / (dr -dl));
		} else {
			max_t = std::min(max_t, (l - r) / (dr -dl));
		}
	};

	std::vector<std::pair<ShadedVertex, ShadedVertex>> vmvn = {{va, vb}, {vb, vc}, {vc, va}};
	std::vector<ShadedVertex> clipped_vertex;

	for (auto v1v2 : vmvn) {
		// reset the t
		min_t = 0.0f;
		max_t = 1.0f;
		ShadedVertex v1 = v1v2.first;
		ShadedVertex v2 = v1v2.second;
		float v1_w = v1.clip_position.w;
		float v2_w = v2.clip_position.w;

		// local names for clip positions and their difference:
		Vec4 const& a = v1.clip_position;
		Vec4 const& b = v2.clip_position;
		Vec4 const ba = b - a;

		// -a.w - t * ba.w <= a.x + t * ba.x <= a.w + t * ba.w
		clip_range(-a.w, -ba.w, a.x, ba.x);
		clip_range(a.x, ba.x, a.w, ba.w);
		// -a.w - t * ba.w <= a.y + t * ba.y <= a.w + t * ba.w
		clip_range(-a.w, -ba.w, a.y, ba.y);
		clip_range(a.y, ba.y, a.w, ba.w);
		// -a.w - t * ba.w <= a.z + t * ba.z <= a.w + t * ba.w
		clip_range(-a.w, -ba.w, a.z, ba.z);
		clip_range(a.z, ba.z, a.w, ba.w);

		if (min_t < max_t) {
			if (min_t == 0.0f) {
				clipped_vertex.push_back(v1);
			} else {
				// default smooth interpolate
				ShadedVertex out = lerp(v1, v2, min_t);
				if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Flat) {
					out.attributes = va.attributes;
				} else if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Correct) {
					for (int i = 0; i < out.attributes.size(); i++) {
						float inv_w = (1.0f - min_t) / v1_w + min_t / v2_w;
						out.attributes[i] = ((v1.attributes[i] / v1_w) * (1 - min_t) + (v2.attributes[i] / v2_w) * min_t) / inv_w;
					}
				}
				clipped_vertex.push_back(out);
			}

			if (max_t == 1.0f) {
				clipped_vertex.push_back(v2);
			} else {
				// default smooth interpolate
				ShadedVertex out = lerp(v1, v2, max_t);
				if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Flat) {
					out.attributes = va.attributes;
				} else if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Correct) {
					for (int i = 0; i < out.attributes.size(); i++) {
						out.attributes[i] = ((v1.attributes[i] / v1.clip_position.w) * max_t + (v2.attributes[i] / v2.clip_position.w) * (1 - max_t)) / (max_t / v1.clip_position.w + (1 - max_t) / v2.clip_position.w);
					}
				}
				clipped_vertex.push_back(out);
			}
		}
	}
	if (clipped_vertex.size() == 3) { // only one triangle
		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[1]);
		emit_vertex(clipped_vertex[2]);
	} else if (clipped_vertex.size() == 4) { // two triangle
		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[1]);
		emit_vertex(clipped_vertex[2]);

		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[2]);
		emit_vertex(clipped_vertex[3]);
	} else if (clipped_vertex.size() == 5) { // three triangle
		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[1]);
		emit_vertex(clipped_vertex[2]);

		emit_vertex(clipped_vertex[2]);
		emit_vertex(clipped_vertex[3]);
		emit_vertex(clipped_vertex[4]);
		
		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[2]);
		emit_vertex(clipped_vertex[4]);
	} else if (clipped_vertex.size() == 6) { // only four triangle
		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[1]);
		emit_vertex(clipped_vertex[2]);

		emit_vertex(clipped_vertex[2]);
		emit_vertex(clipped_vertex[3]);
		emit_vertex(clipped_vertex[4]);
		
		emit_vertex(clipped_vertex[4]);
		emit_vertex(clipped_vertex[5]);
		emit_vertex(clipped_vertex[0]);
					
		emit_vertex(clipped_vertex[0]);
		emit_vertex(clipped_vertex[2]);
		emit_vertex(clipped_vertex[4]);
	}
}

// -------------------------------------------------------------------------
// rasterization functions

/*
 * rasterize_line:
 * calls emit_fragment( frag ) for every pixel "covered" by the line (va.fb_position.xy, vb.fb_position.xy).
 *
 *    a pixel (x,y) is "covered" by the line if it exits the inscribed diamond:
 * 
 *        (x+0.5,y+1)
 *        /        \
 *    (x,y+0.5)  (x+1,y+0.5)
 *        \        /
 *         (x+0.5,y)
 *
 *    to avoid ambiguity, we consider diamonds to contain their left and bottom points
 *    but not their top and right points. 
 * 
 * 	  since 45 degree lines breaks this rule, our rule in general is to rasterize the line as if its
 *    endpoints va and vb were at va + (e, e^2) and vb + (e, e^2) where no smaller nonzero e produces 
 *    a different rasterization result. 
 *    We will not explicitly check for 45 degree lines along the diamond edges (this will be extra credit),
 *    but you should be able to handle 45 degree lines in every other case (such as starting from pixel centers)
 *
 * for each such diamond, pass Fragment frag to emit_fragment, with:
 *  - frag.fb_position.xy set to the center (x+0.5,y+0.5)
 *  - frag.fb_position.z interpolated linearly between va.fb_position.z and vb.fb_position.z
 *  - frag.attributes set to va.attributes (line will only be used in Interp_Flat mode)
 *  - frag.derivatives set to all (0,0)
 *
 * when interpolating the depth (z) for the fragments, you may use any depth the line takes within the pixel
 * (i.e., you don't need to interpolate to, say, the closest point to the pixel center)
 *
 * If you wish to work in fixed point, check framebuffer.h for useful information about the framebuffer's dimensions.
 */


namespace std {
    template <>
    struct hash<std::pair<float, float>> {
        size_t operator()(const std::pair<float, float>& p) const {
            size_t h1 = std::hash<float>{}(p.first);
            size_t h2 = std::hash<float>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
}

template<PrimitiveType p, class P, uint32_t flags>
void Pipeline<p, P, flags>::rasterize_line(
	ClippedVertex const& va, ClippedVertex const& vb,
	std::function<void(Fragment const&)> const& emit_fragment, 
	float const& super_sampleing_dx, float const& super_sampleing_dy) {
	if constexpr ((flags & PipelineMask_Interp) != Pipeline_Interp_Flat) {
		assert(0 && "rasterize_line should only be invoked in flat interpolation mode.");
	}
	// A1T2: rasterize_line

	// TODO: Check out the block comment above this function for more information on how to fill in
	// this function!
	// The OpenGL specification section 3.5 may also come in handy.

	auto va_copy = va;
	auto vb_copy = vb;
	// Point A should always be the left one
	if (va.fb_position.x > vb.fb_position.x) {
		std::swap(va_copy, vb_copy);
	}

	// define coordinates
	float va_x = va_copy.fb_position.x;
	float va_y = va_copy.fb_position.y;
	float vb_x = vb_copy.fb_position.x;
	float vb_y = vb_copy.fb_position.y;

	auto draw_point = [&](const float x, const float y) {
		Fragment mid;
		float t = vb_x == va_x ? (x - va_x) / (vb_x - va_x) : (y - va_y) / (vb_y - va_y);
		float z = (1 - t) * va_copy.fb_position.z + t * vb_copy.fb_position.z;
		mid.fb_position = Vec3(x, y, z);
		mid.attributes = va_copy.attributes;
		mid.derivatives.fill(Vec2(0.0f, 0.0f));
		emit_fragment(mid);
	};

	auto check_diamond = [&](const float p_x, const float p_y) {
		float x = std::floor(p_x) + 0.5f;
		float y = std::floor(p_y) + 0.5f;
		// check top and right point
		if ((p_x == x && p_y == y + 0.5) || (p_x == x + 0.5 && p_y == y)) {
			return false;
		} else if (abs(x - p_x) + abs(y - p_y) <= 0.5) {
			return true;
		}
		return false;
	};

	std::unordered_set<std::pair<float, float>> raster_points;

	// vertical
	if (va_x == vb_x && va_y != vb_y) {
		float y = va_y;
		float x = va_x;
		float direction = va_y < vb_y ? 1.0f : -1.0f;
		while (true) {
			raster_points.insert({std::floor(x) + super_sampleing_dx, std::floor(y) + super_sampleing_dy});
			y += direction;
			if ((direction == 1 && y >= vb_y) || (direction == -1 && y <= vb_y)) {
				break;
			}
		}
	} 
	// horizontal
	else if (va_x != vb_x && va_y == vb_y) {
		float y = va_y;
		float x = va_x;
		while (true) {
			raster_points.insert({std::floor(x) + super_sampleing_dx, std::floor(y) + super_sampleing_dy});
			x++;
			if (x >= vb_x) {
				break;
			}
		}
	}
	// start point and end point are the same point
	else if (va_x == vb_x && va_y == vb_y) {
		throw std::runtime_error("Error: Start point and end point are the same!");
	} 
	// normal condition
	else {
		float eps = 0;
		float m = (vb_y - va_y) / (vb_x - va_x);	// slope
		if (m == 1.0f) {
			float e = 0.0001f;
			va_x += e;
			vb_x += e;
			va_y += e * e;
			vb_y += e * e;
		}
		
		if (abs(m) <= 1) {
			float y = va_y;
			for (float x = va_x; x <=vb_x; x++) {
				// check the diamond
				if (check_diamond(x, y)) {
					raster_points.insert({std::floor(x) + super_sampleing_dx, std::floor(y) + super_sampleing_dy});
				}
				eps += m;
				if (m >= 0 && eps > 0.5) {
					y++;
					eps--;
				} else if (m < 0 && eps < -0.5) {
					y--;
					eps++;
				}
				
			}
		} else {
			float x = va_x;
			for (float y = va_y; y <=vb_y; y++) {
				// check the diamond
				if (check_diamond(x, y)) {
					raster_points.insert({std::floor(x)  + super_sampleing_dx, std::floor(y) + super_sampleing_dy});
				}
				eps += 1 / m;
				if (m >= 0 && eps > 0.5) {
					x++;
					eps--;
				} else if (m < 0 && eps < -0.5) {
					x--;
					eps++;
				}
				
			}
		}
	}

	// remove the fragment of endpoint
	if (check_diamond(vb_x, vb_y)) {
		std::pair<float, float> last_point = {std::floor(vb_x) + super_sampleing_dx, std::floor(vb_y) + super_sampleing_dy};
		auto it = raster_points.find(last_point);
		if (it != raster_points.end()) {
			raster_points.erase(it);
		}
	}

	// draw all point
	for (const auto& point : raster_points) {
		draw_point(point.first, point.second);
	}
}

/*
 * rasterize_triangle(a,b,c,emit) calls 'emit(frag)' at every location
 *  	(x+0.5,y+0.5) (where x,y are integers) covered by triangle (a,b,c).
 *
 * The emitted fragment should have:
 * - frag.fb_position.xy = (x+0.5, y+0.5)
 * - frag.fb_position.z = linearly interpolated fb_position.z from a,b,c (NOTE: does not depend on Interp mode!)
 * - frag.attributes = depends on Interp_* flag in flags:
 *   - if Interp_Flat: copy from va.attributes
 *   - if Interp_Smooth: interpolate as if (a,b,c) is a 2D triangle flat on the screen
 *   - if Interp_Correct: use perspective-correct interpolation
 * - frag.derivatives = derivatives w.r.t. fb_position.x and fb_position.y of the first frag.derivatives.size() attributes.
 *
 * Notes on derivatives:
 * 	The derivatives are partial derivatives w.r.t. screen locations. That is:
 *    derivatives[i].x = d/d(fb_position.x) attributes[i]
 *    derivatives[i].y = d/d(fb_position.y) attributes[i]
 *  You may compute these derivatives analytically or numerically.
 *
 *  See section 8.12.1 "Derivative Functions" of the GLSL 4.20 specification for some inspiration. (*HOWEVER*, the spec is solving a harder problem, and also nothing in the spec is binding on your implementation)
 *
 *  One approach is to rasterize blocks of four fragments and use forward and backward differences to compute derivatives.
 *  To assist you in this approach, keep in mind that the framebuffer size is *guaranteed* to be even. (see framebuffer.h)
 *
 * Notes on coverage:
 *  If two triangles are on opposite sides of the same edge, and a
 *  fragment center lies on that edge, rasterize_triangle should
 *  make sure that exactly one of the triangles emits that fragment.
 *  (Otherwise, speckles or cracks can appear in the final render.)
 * 
 *  For degenerate (co-linear) triangles, you may consider them to not be on any side of an edge.
 * 	Thus, even if two degnerate triangles share an edge that contains a fragment center, you don't need to emit it.
 *  You will not lose points for doing something reasonable when handling this case
 *
 *  This is pretty tricky to get exactly right!
 *
 */
template<PrimitiveType p, class P, uint32_t flags>
void Pipeline<p, P, flags>::rasterize_triangle(
	ClippedVertex const& va, ClippedVertex const& vb, ClippedVertex const& vc,
	std::function<void(Fragment const&)> const& emit_fragment,
	// float const& super_sampleing_dx, float const& super_sampleing_dy) {
	std::vector<Vec3> samples) {
	// NOTE: it is okay to restructure this function to allow these tasks to use the
	//  same code paths. Be aware, however, that all of them need to remain working!
	//  (e.g., if you break Flat while implementing Correct, you won't get points
	//   for Flat.)
	
	// A1T3: flat triangles
	// TODO: rasterize triangle (see block comment above this function).

	// use the cross product to determine CCW or CW

	/**
	 * Preparation
	 */
	if (samples.size() == 0) {
		samples.push_back(Vec3(.5f, .5f, .0f));
	}
	float cross_product = (vb.fb_position.x - va.fb_position.x) * (vc.fb_position.y - va.fb_position.y) - (vb.fb_position.y - va.fb_position.y) * (vc.fb_position.x - va.fb_position.x);
	const float area = std::abs(cross_product);
	bool CW;
	if (cross_product > 0) CW = false;		// CCW
	else if (cross_product < 0) CW = true;	// CW
	else return;							// degenerate triangle

	// judge top left edge
	std::function<bool(const ClippedVertex&, const ClippedVertex&)> top_left_edge;

	if (CW) {
		top_left_edge = [](const ClippedVertex& v1, const ClippedVertex& v2) {
			return v2.fb_position.y > v1.fb_position.y || (v1.fb_position.y == v2.fb_position.y && v2.fb_position.x > v1.fb_position.x);
		};
	} else {
		top_left_edge = [](const ClippedVertex& v1, const ClippedVertex& v2) {
			return v2.fb_position.y < v1.fb_position.y || (v1.fb_position.y == v2.fb_position.y && v2.fb_position.x < v1.fb_position.x);
		};
	}
	
	float min_x = std::floor((std::min(std::min(va.fb_position.x, vb.fb_position.x), vc.fb_position.x)));
	float min_y = std::floor((std::min(std::min(va.fb_position.y, vb.fb_position.y), vc.fb_position.y)));
	float max_x = std::floor((std::max(std::max(va.fb_position.x, vb.fb_position.x), vc.fb_position.x))) + 1;
	float max_y = std::floor((std::max(std::max(va.fb_position.y, vb.fb_position.y), vc.fb_position.y))) + 1;

	/**
	 * 
	 * Judge whether in the triangle or not
	 * 
	 */
	const std::array<std::pair<ClippedVertex, ClippedVertex>, 3> edge_pairs = {{ {va, vb}, {vb, vc}, {vc, va} }};
	std::vector<float> cross_products; // remeber to clear it after use
	auto in_triangle = [&](float x, float y){
		const Vec2 point = Vec2(x, y);
		for (auto& pair : edge_pairs) {
			ClippedVertex v1 = pair.first;
			ClippedVertex v2 = pair.second;
			float v1_x = v1.fb_position.x;
			float v1_y = v1.fb_position.y;
			float v2_x = v2.fb_position.x;
			float v2_y = v2.fb_position.y;
			float p_cross_product = (v2_x - v1_x) * (point.y - v1_y) - (v2_y - v1_y) * (point.x - v1_x);
			bool in;
			if (top_left_edge(v1, v2)) {
				in = CW ? p_cross_product <= 0 : p_cross_product >= 0; 
			} else {
				in = CW ? p_cross_product < 0 : p_cross_product > 0;
			}
			if (!in) {
				continue;
			} else {
				cross_products.push_back(std::abs(p_cross_product));
			}
		}
		
		if (cross_products.size() != 3) {
			return false;
		} else {
			return true;
		}
	};

	if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Flat) {
		for (float x = min_x; x <= max_x; x++) {
			for (float y =min_y; y <= max_y; y++) {
				bool rasterize_or_not = false;
				std::vector<Fragment> frags;
				for (uint32_t s = 0; s < samples.size(); ++s) {
					float super_sampleing_dx = samples[s].x;
					float super_sampleing_dy = samples[s].y;
					float sampled_x = x + super_sampleing_dx;
					float sampled_y = y + super_sampleing_dy;		
					Fragment frag;
					if (in_triangle(sampled_x, sampled_y)) {
						rasterize_or_not = true;	
						// interpolated z
						// c -> a -> b
						float c = cross_products[0] / area;
						float a = cross_products[1] / area;
						float b = cross_products[2] / area;
						float za = va.fb_position.z;
						float zb = vb.fb_position.z;
						float zc = vc.fb_position.z;
						float z_d = a * za + b * zb + c * zc;
						frag.fb_position = Vec3(sampled_x, sampled_y, z_d);
						frag.attributes = va.attributes;
						// Using flat interpolation, I don't need to calculate the derivatives :)
						frag.derivatives.fill(Vec2(0.0f, 0.0f));
					} else {
						frag.fb_position = Vec3(sampled_x, sampled_y, 1.0f);
						for (int i = 0; i < frag.attributes.size(); i++) {
							frag.attributes[i] = .0f;
						}
						for (int i = 0; i < frag.derivatives.size(); i++) {
							frag.derivatives[i] = Vec2(.0f, .0f);
						}
					}
					frags.push_back(frag);
					cross_products.clear(); 
				}
				if (rasterize_or_not) {
					for (auto frag : frags) {
						emit_fragment(frag);
					}
				}
				rasterize_or_not = false;
			}
		}
	} else if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Smooth) {
		// A1T5: screen-space smooth triangles
		// TODO: rasterize triangle (see block comment above this function).

		// As a placeholder, here's code that calls the Flat interpolation version of the function:
		//(remove this and replace it with a real solution)
		for (float x = min_x; x <= max_x; x++) {
			for (float y =min_y; y <= max_y; y++) {
				bool rasterize_or_not = false;
				std::vector<Fragment> frags;
				for (uint32_t s = 0; s < samples.size(); ++s) {
					float super_sampleing_dx = samples[s].x;
					float super_sampleing_dy = samples[s].y;
					float sampled_x = x + super_sampleing_dx;
					float sampled_y = y + super_sampleing_dy;	
					Fragment frag;
					if (in_triangle(sampled_x, sampled_y)){
						rasterize_or_not = true;	
						// interpolated z
						// c -> a -> b
						float c = cross_products[0] / area;
						float a = cross_products[1] / area;
						float b = cross_products[2] / area;
						float za = va.fb_position.z;
						float zb = vb.fb_position.z;
						float zc = vc.fb_position.z;
						// float lambda_a = va.inv_w * a; // inv_w is (1/w)
						// float lambda_b = vb.inv_w * b;
						// float lambda_c = vc.inv_w * c;
						// float z_d = (lambda_a + lambda_b + lambda_c) / ((lambda_a / za) + (lambda_b / zb) + (lambda_c / zc));
						float z_d = a * za + b * zb + c * zc;

						frag.fb_position = Vec3(sampled_x, sampled_y, z_d);
						for (int i = 0; i < frag.attributes.size(); i++) {
							float f_a = va.attributes[i];
							float f_b = vb.attributes[i];
							float f_c = vc.attributes[i];
							frag.attributes[i] = a * f_a + b * f_b + c * f_c;
						}
						// NOTE: here use Cramer formula to get the derivatives
						for (int i = 0; i < frag.derivatives.size(); i++) {
							float f_a = va.attributes[i];
							float f_b = vb.attributes[i];
							float f_c = vc.attributes[i];
							float dfdx = ((f_b - f_a) * (vc.fb_position.y - va.fb_position.y) - (f_c - f_a) * (vb.fb_position.y - va.fb_position.y)) / area;
							float dfdy = ((f_c - f_a) * (vb.fb_position.x - va.fb_position.x) - (f_b - f_a) * (vc.fb_position.x - va.fb_position.x)) / area;
							frag.derivatives[i] = Vec2(dfdx, dfdy);
						}
					} else {
						frag.fb_position = Vec3(sampled_x, sampled_y, 1.0f);
						for (int i = 0; i < frag.attributes.size(); i++) {
							frag.attributes[i] = .0f;
						}
						for (int i = 0; i < frag.derivatives.size(); i++) {
							frag.derivatives[i] = Vec2(.0f, .0f);
						}
					}
					frags.push_back(frag);
					cross_products.clear(); 
				}
				if (rasterize_or_not) {
					for (auto frag : frags) {
						emit_fragment(frag);
					}
				}
				rasterize_or_not = false;
			}
		}
	} else if constexpr ((flags & PipelineMask_Interp) == Pipeline_Interp_Correct) {
		// A1T5: perspective correct triangles
		// TODO: rasterize triangle (block comment above this function).

		// As a placeholder, here's code that calls the Screen-space interpolation function:
		//(remove this and replace it with a real solution)

		for (float x = min_x; x <= max_x; x++) {
			for (float y =min_y; y <= max_y; y++) {
				bool rasterize_or_not = false;
				std::vector<Fragment> frags;
				for (uint32_t s = 0; s < samples.size(); ++s) {
					float super_sampleing_dx = samples[s].x;
					float super_sampleing_dy = samples[s].y;
					float sampled_x = x + super_sampleing_dx;
					float sampled_y = y + super_sampleing_dy;	
					Fragment frag;
					if (in_triangle(sampled_x, sampled_y)) {
						rasterize_or_not = true;	
						// c -> a -> b
						float c = cross_products[0] / area;
						float a = cross_products[1] / area;
						float b = cross_products[2] / area;
						// inv_w is (1 / w)
						// interpolate (1 / w) 
						float inv_w_interpolated = a * va.inv_w + b * vb.inv_w + c * vc.inv_w;
						// interpolate z
						float z_d = a * va.fb_position.z + b * vb.fb_position.z + c * vc.fb_position.z;
						frag.fb_position = Vec3(sampled_x, sampled_y, z_d);
						// interpolate (attri / w)
						for (int i = 0; i < frag.attributes.size(); i++) {
							frag.attributes[i] = ((va.attributes[i] * va.inv_w) * a + (vb.attributes[i] * vb.inv_w) * b + (vc.attributes[i] * vc.inv_w) * c) / inv_w_interpolated;
						}
						// NOTE: here use Cramer formula to get the derivatives
						/*
						f / w = Ax + By + C
						1 / w = Dx + Ey + F
						f = (f/w) / (1/w) = g / h
						df/dx = d(g/h)/dx (remeber use g = fh to replace g)
						
						*/
						float inv_w_a = va.inv_w;
						float inv_w_b = vb.inv_w;
						float inv_w_c = vc.inv_w;
						float dinv_w_dx = ((inv_w_b - inv_w_a) * (vc.fb_position.y - va.fb_position.y) - (inv_w_c - inv_w_a) * (vb.fb_position.y - va.fb_position.y)) / area;
						float dinv_w_dy = ((inv_w_c - inv_w_a) * (vb.fb_position.x - va.fb_position.x) - (inv_w_b - inv_w_a) * (vc.fb_position.x - va.fb_position.x)) / area;
		
						float inv_w = inv_w_interpolated;
		
						for (int i = 0; i < frag.derivatives.size(); i++) {
							float fa = va.attributes[i] * va.inv_w;
							float fb = vb.attributes[i] * vb.inv_w;
							float fc = vc.attributes[i] * vc.inv_w;
		
							float d_attr_over_w_dx = ((fb - fa) * (vc.fb_position.y - va.fb_position.y) - (fc - fa) * (vb.fb_position.y - va.fb_position.y)) / area;
							float d_attr_over_w_dy = ((fc - fa) * (vb.fb_position.x - va.fb_position.x) - (fb - fa) * (vc.fb_position.x - va.fb_position.x)) / area;
		
							float attr = frag.attributes[i];
		
							float dfdx = (d_attr_over_w_dx - attr * dinv_w_dx) / inv_w;
							float dfdy = (d_attr_over_w_dy - attr * dinv_w_dy) / inv_w;
		
							frag.derivatives[i] = Vec2(dfdx, dfdy);
						}
					} else {
						frag.fb_position = Vec3(sampled_x, sampled_y, 1.0f);
						for (int i = 0; i < frag.attributes.size(); i++) {
							frag.attributes[i] = .0f;
						}
						for (int i = 0; i < frag.derivatives.size(); i++) {
							frag.derivatives[i] = Vec2(.0f, .0f);
						}
					}
					frags.push_back(frag);
					cross_products.clear(); 
				}
				if (rasterize_or_not) {
					for (auto frag : frags) {
						emit_fragment(frag);
					}
				}
				rasterize_or_not = false;
			}
		}
	}
}

//-------------------------------------------------------------------------
// compile instantiations for all programs and blending and testing types:

#include "programs.h"

template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Always | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Always | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Always | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Never | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Never | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Never | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Less | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Less | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Less | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Always | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Always | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Always | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Never | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Never | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Never | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Less | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Less | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Less | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Always | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Always | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Always | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Never | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Never | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Never | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Less | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Less | Pipeline_Interp_Smooth>;
template struct Pipeline<PrimitiveType::Triangles, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Less | Pipeline_Interp_Correct>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Always | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Never | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Replace | Pipeline_Depth_Less | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Always | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Never | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Add | Pipeline_Depth_Less | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Always | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Never | Pipeline_Interp_Flat>;
template struct Pipeline<PrimitiveType::Lines, Programs::Lambertian,
                         Pipeline_Blend_Over | Pipeline_Depth_Less | Pipeline_Interp_Flat>;