
#include "samplers.h"
#include "../util/rand.h"

constexpr bool IMPORTANCE_SAMPLING = true;

namespace Samplers {

Vec2 Rect::sample(RNG &rng) const {
	//A3T1 - step 2 - supersampling

    // Return a point selected uniformly at random from the rectangle [0,size.x)x[0,size.y)
    // Useful function: rng.unit()

    return Vec2(size.x * rng.unit(), size.y * rng.unit());
}

float Rect::pdf(Vec2 at) const {
	if (at.x < 0.0f || at.x > size.x || at.y < 0.0f || at.y > size.y) return 0.0f;
	return 1.0f / (size.x * size.y);
}

Vec2 Circle::sample(RNG &rng) const {
	//A3EC - bokeh - circle sampling

    // Return a point selected uniformly at random from a circle defined by its
	// center and radius.
    // Useful function: rng.unit()

    return Vec2{};
}

float Circle::pdf(Vec2 at) const {
	//A3EC - bokeh - circle pdf

	// Return the pdf of sampling the point 'at' for a circle defined by its
	// center and radius.

    return 1.f;
}

Vec3 Point::sample(RNG &rng) const {
	return point;
}

float Point::pdf(Vec3 at) const {
	return at == point ? 1.0f : 0.0f;
}

Vec3 Triangle::sample(RNG &rng) const {
	float u = std::sqrt(rng.unit());
	float v = rng.unit();
	float a = u * (1.0f - v);
	float b = u * v;
	return a * v0 + b * v1 + (1.0f - a - b) * v2;
}

float Triangle::pdf(Vec3 at) const {
	float a = 0.5f * cross(v1 - v0, v2 - v0).norm();
	float u = 0.5f * cross(at - v1, at - v2).norm() / a;
	float v = 0.5f * cross(at - v2, at - v0).norm() / a;
	float w = 1.0f - u - v;
	if (u < 0.0f || v < 0.0f || w < 0.0f) return 0.0f;
	if (u > 1.0f || v > 1.0f || w > 1.0f) return 0.0f;
	return 1.0f / a;
}

Vec3 Hemisphere::Uniform::sample(RNG &rng) const {

	float Xi1 = rng.unit();
	float Xi2 = rng.unit();

	float theta = std::acos(Xi1);
	float phi = 2.0f * PI_F * Xi2;

	float xs = std::sin(theta) * std::cos(phi);
	float ys = std::cos(theta);
	float zs = std::sin(theta) * std::sin(phi);

	return Vec3(xs, ys, zs);
}

float Hemisphere::Uniform::pdf(Vec3 dir) const {
	if (dir.y < 0.0f) return 0.0f;
	return 1.0f / (2.0f * PI_F);
}

Vec3 Hemisphere::Cosine::sample(RNG &rng) const {

	float phi = rng.unit() * 2.0f * PI_F;
	float cos_t = std::sqrt(rng.unit());

	float sin_t = std::sqrt(1 - cos_t * cos_t);
	float x = std::cos(phi) * sin_t;
	float z = std::sin(phi) * sin_t;
	float y = cos_t;

	return Vec3(x, y, z);
}

float Hemisphere::Cosine::pdf(Vec3 dir) const {
	if (dir.y < 0.0f) return 0.0f;
	return dir.y / PI_F;
}

Vec3 Sphere::Uniform::sample(RNG &rng) const {
	//A3T7 - sphere sampler

    // Generate a uniformly random point on the unit sphere.
    // Tip: start with Hemisphere::Uniform

	float Xi1 = rng.unit();
	float Xi2 = rng.unit();

	float theta = std::acos(1 - 2 * Xi1);
	float phi = 2.0f * PI_F * Xi2;

	float xs = std::sin(theta) * std::cos(phi);
	float ys = std::cos(theta);
	float zs = std::sin(theta) * std::sin(phi);
	return Vec3(xs, ys, zs);
}

float Sphere::Uniform::pdf(Vec3 dir) const {
	return 1.0f / (4.0f * PI_F);
}

Sphere::Image::Image(const HDR_Image& image) {
    //A3T7 - image sampler init

    // Set up importance sampling data structures for a spherical environment map image.
    // You may make use of the _pdf, _cdf, and total members, or create your own.

    const auto [_w, _h] = image.dimension();
    w = _w;
    h = _h;
	_pdf.reserve(w * h);
	_cdf.reserve(w * h);
	float pdf_sum = 0.f;
	for (uint32_t i = 0; i < w * h; ++i) {
		uint32_t row = i / w;
		float theta = PI_F * row / (float)h; 
		float sin_theta = std::sin(theta);

		float weighted_luma = image.at(i).luma() * sin_theta;
		_pdf.push_back(weighted_luma);
		pdf_sum += weighted_luma;
	}

	for (auto& p : _pdf) {
		p /= pdf_sum;
	}

	_cdf.push_back(_pdf[0]);
	for (uint32_t i = 1; i < w * h; ++i) {
		_cdf.push_back(_cdf[i - 1] + _pdf[i]);
	}
	_cdf.back() = 1.0f; // Ensure the last value is exactly 1.0
}

Vec3 Sphere::Image::sample(RNG &rng) const {
	if(!IMPORTANCE_SAMPLING) {
		// Step 1: Uniform sampling
		// Declare a uniform sampler and return its sample
		Uniform uniform = Uniform{};
		Vec3 sample = uniform.sample(rng);
    	return sample;
	} else {
		// Step 2: Importance sampling
		// Use your importance sampling data structure to generate a sample direction.
		// Tip: std::upper_bound
		float X = rng.unit();
		auto it = std::upper_bound(_cdf.begin(), _cdf.end(), X);
		__int64 index = std::distance(_cdf.begin(), it);
		__int64 row = index / w;
		__int64 col = index % w;

		float theta = row / h * PI_F; 
		float phi = col / w * 2 * PI_F;
		
		float xs = std::sin(theta) * std::cos(phi);
		float ys = std::cos(theta);
		float zs = std::sin(theta) * std::sin(phi);
		
    	return Vec3{xs, ys, zs};
	}
}

float Sphere::Image::pdf(Vec3 dir) const {
    if(!IMPORTANCE_SAMPLING) {
		// Step 1: Uniform sampling
		// Declare a uniform sampler and return its pdf
		Uniform uniform = Uniform{};
		return uniform.pdf(dir);
	} else {
		// A3T7 - image sampler importance sampling pdf
		// What is the PDF of this distribution at a particular direction?
		float theta = std::acos(dir.y);
		float phi = std::atan2(dir.z, dir.x);
		if (phi < 0.0f) {
			phi += 2 * PI_F;
		}
		float row = theta / PI_F * h;
		float col = phi / (2 * PI_F) * w;
		float base = _pdf[(int)row * w + (int)col];
		float sin_theta = std::sin(theta);
		if (sin_theta <= EPS_F) {
			return 0.0f;
		}
		float jacobian = (w * h) / (2 * PI_F * PI_F * sin_theta);
		return base * jacobian;
	}
}

} // namespace Samplers
