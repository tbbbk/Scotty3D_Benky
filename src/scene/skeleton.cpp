#include <unordered_set>
#include "skeleton.h"
#include "test.h"
#include <iostream>

void Skeleton::Bone::compute_rotation_axes(Vec3 *x_, Vec3 *y_, Vec3 *z_) const {
	assert(x_ && y_ && z_);
	auto &x = *x_;
	auto &y = *y_;
	auto &z = *z_;

	//y axis points in the direction of extent:
	y = extent.unit();
	//if extent is too short to normalize nicely, point along the skeleton's 'y' axis:
	if (!y.valid()) {
		y = Vec3{0.0f, 1.0f, 0.0f};
	}

	//x gets skeleton's 'x' axis projected to be orthogonal to 'y':
	x = Vec3{1.0f, 0.0f, 0.0f};
	x = (x - dot(x,y) * y).unit();
	if (!x.valid()) {
		//if y perfectly aligns with skeleton's 'x' axis, x, gets skeleton's z axis:
		x = Vec3{0.0f, 0.0f, 1.0f};
		x = (x - dot(x,y) * y).unit(); //(this should do nothing)
	}

	//z computed from x,y:
	z = cross(x,y);

	//x,z rotated by roll:
	float cr = std::cos(roll / 180.0f * PI_F);
	float sr = std::sin(roll / 180.0f * PI_F);
	// x = cr * x + sr * -z;
	// z = cross(x,y);
	std::tie(x, z) = std::make_pair(cr * x + sr * -z, cr * z + sr * x);
}

std::vector< Mat4 > Skeleton::bind_pose() const {
	//A4T2a: bone-to-skeleton transformations in the bind pose
	//(the bind pose does not rotate by Bone::pose)

	std::vector< Mat4 > bind;
	bind.reserve(bones.size());

	//NOTE: bones is guaranteed to be ordered such that parents appear before child bones.

	for (auto const &bone : bones) {
		(void)bone; //avoid complaints about unused bone
		//placeholder -- your code should actually compute the correct transform:
		if (bone.parent == -1U) {
			//base bone (no parent) is at the skeleton's base position:
			bind.emplace_back(Mat4::translate(base));
			continue;
		} 
		//child bone is at the parent's tip:
		// parent's bind transform is in bind[bone.parent]:
		bind.emplace_back(bind[bone.parent] * Mat4::translate(bones[bone.parent].extent));
	}

	assert(bind.size() == bones.size()); //should have a transform for every bone.
	return bind;
}

std::vector< Mat4 > Skeleton::current_pose() const {
    //A4T2a: bone-to-skeleton transformations in the current pose
	std::vector< Mat4 > pose;
	pose.reserve(bones.size());
	//Similar to bind_pose(), but takes rotation from Bone::pose into account.
	// (and translation from Skeleton::base_offset!)

	//You'll probably want to write a loop similar to bind_pose().
	//Useful functions:
	//Bone::compute_rotation_axes() will tell you what axes (in local bone space) Bone::pose should rotate around.
	//Mat4::angle_axis(angle, axis) will produce a matrix that rotates angle (in degrees) around a given axis.
	for (auto const &bone : bones) {
		Mat4 parent_transform;
		if (bone.parent == -1U) {
			//base bone (no parent) is at the skeleton's base position:
			parent_transform = Mat4::translate(base + base_offset);
		} else {
			//child bone is at the parent's tip:
			parent_transform = pose[bone.parent];
		}

		Mat4 T = Mat4::translate((bone.parent == -1U) ? Vec3(.0f) : bones[bone.parent].extent);

		Vec3 x_axis, y_axis, z_axis;
		bone.compute_rotation_axes(&x_axis, &y_axis, &z_axis);
		Mat4 rotation_x = Mat4::angle_axis(bone.pose.x, x_axis);
		Mat4 rotation_y = Mat4::angle_axis(bone.pose.y, y_axis);
		Mat4 rotation_z = Mat4::angle_axis(bone.pose.z, z_axis);
		pose.emplace_back(parent_transform * T * rotation_z * rotation_y * rotation_x);
	}

	assert(pose.size() == bones.size()); //should have a transform for every bone.
	return pose;

}

std::vector< Vec3 > Skeleton::gradient_in_current_pose() const {
    //A4T2b: IK gradient

    // Computes the gradient (partial derivative) of IK energy relative to each bone's Bone::pose, in the current pose.

	//The IK energy is the sum over all *enabled* handles of the squared distance from the tip of Handle::bone to Handle::target
	std::vector< Vec3 > gradient(bones.size(), Vec3{0.0f, 0.0f, 0.0f});

	//TODO: loop over handles and over bones in the chain leading to the handle, accumulating gradient contributions.
	//remember bone.compute_rotation_axes() -- should be useful here, too!
	std::vector<Mat4> current_pose = this->current_pose();

	for (auto const &handle : handles) {
		if (!handle.enabled) {
			continue; //skip if handle is not enabled
		}
		for (BoneIndex b = handle.bone; b != -1U; b = bones[b].parent) {
			Bone bone = bones[b];
			Vec3 x_axis, y_axis, z_axis;  // the axis is in **local space**
			bone.compute_rotation_axes(&x_axis, &y_axis, &z_axis);
			
			Vec3 loss = (current_pose[handle.bone] * Vec4(bone.extent, 1.f)).xyz() - handle.target;

			// The bone's beginning
			Vec4 r = current_pose[handle.bone] * Vec4(.0f, .0f, .0f, 1.0f);
			Vec4 p = current_pose[handle.bone] * Vec4(bone.extent, 1.0f);
			
			Vec4 x_axis_skeleton_space = current_pose[handle.bone] * Vec4(1.f, 0.f, 0.f, 0.f);
			Vec3 dp_drx = cross(x_axis_skeleton_space.xyz(), (p - r).xyz());

			Mat4 rotation_x_reverse = Mat4::transpose(Mat4::angle_axis(bone.pose.x, x_axis));
			Vec4 y_axis_skeleton_space = current_pose[handle.bone] * rotation_x_reverse * Vec4(0.f, 1.f, 0.f, 0.f);
			Vec3 dp_dry = cross(y_axis_skeleton_space.xyz(), (p - r).xyz());

			Mat4 rotation_y_reverse = Mat4::transpose(Mat4::angle_axis(bone.pose.y, y_axis));
			Vec4 z_axis_skeleton_space = current_pose[handle.bone] * rotation_y_reverse * rotation_x_reverse * Vec4(0.f, 0.f, 1.f, 0.f);
			Vec3 dp_drz = cross(z_axis_skeleton_space.xyz(), (p - r).xyz());

			float dx = dot(loss, dp_drx);
			float dy = dot(loss, dp_dry);
			float dz = dot(loss, dp_drz);
			gradient[handle.bone] += Vec3{dx, dy, dz};
		}
	}

	assert(gradient.size() == bones.size());
	return gradient;
}

bool Skeleton::solve_ik(uint32_t steps) {
	//A4T2b - gradient descent
	//check which handles are enabled
	//run `steps` iterations
	float tao = 0.05f;
	for (uint32_t step = 0; step < steps; ++step) {
		//call gradient_in_current_pose() to compute d loss / d pose
		//add ...
		std::vector< Vec3 > gradient = gradient_in_current_pose();

		bool at_local_minimum = true; //assume we are at a local minimum
		for (auto const &g : gradient) {
			if (std::abs(g.x) >= 0.0001 || std::abs(g.y) >= 0.0001 || std::abs(g.z) >= 0.0001) {
				at_local_minimum = false; //if any component of the gradient is not near-zero, we are not at a local minimum
				break;
			}
		}

		if (at_local_minimum) {
			return true; //if we are at a local minimum, return 'true'
		}

		for (BoneIndex b = 0; b < bones.size(); ++b) {
			bones[b].pose -= gradient[b] * tao; 
		}
	}
	
	//if at a local minimum (e.g., gradient is near-zero), return 'true'.
	//if run through all steps, return `false`.
	return false;
}

Vec3 Skeleton::closest_point_on_line_segment(Vec3 const &a, Vec3 const &b, Vec3 const &p) {
	//A4T3: bone weight computation (closest point helper)

    // Return the closest point to 'p' on the line segment from a to b

	//Efficiency note: you can do this without any sqrt's! (no .unit() or .norm() is needed!)
	Vec3 ab = b - a; // vector from a to b
	Vec3 ap = p - a; // vector from p to a
	float x = dot(ap, ab) / dot(ab, ab);

	x = std::clamp(x, 0.0f, 1.0f); // clamp x to [0, 1] to ensure we stay on the segment

    return Vec3{a + ab * x}; // return the point on the line segment
}

void Skeleton::assign_bone_weights(Halfedge_Mesh *mesh_) const {
	assert(mesh_);
	auto &mesh = *mesh_;
	(void)mesh; //avoid complaints about unused mesh

	//A4T3: bone weight computation

	//visit every vertex and **set new values** in Vertex::bone_weights (don't append to old values)

	//be sure to use bone positions in the bind pose (not the current pose!)

	//you should fill in the helper closest_point_on_line_segment() before working on this function
	
	std::vector<Mat4> bind_to_pose = bind_pose(); //make sure bind pose is computed
	for (auto &v : mesh.vertices) {
		float weight_sum = 0.0f; // sum of weights for this vertex
		std::vector< std::pair< BoneIndex, float > > new_weights; // new weights to be assigned

		v.bone_weights.clear(); // clear old weights
		for (BoneIndex bone_idx = 0; bone_idx < bones.size(); ++bone_idx) {
			const Bone &bone = bones[bone_idx];
			Vec3 start = (bind_to_pose[bone_idx] * Vec4(.0f, .0f, .0f, 1.0f)).xyz(); //start of the bone in the bind pose
			Vec3 end = (bind_to_pose[bone_idx] * Vec4(bone.extent, 1.0f)).xyz(); //end of the bone in the bind pose
			Vec3 p = v.position; //position of the vertex in the bind pose
			Vec3 closest = closest_point_on_line_segment(start, end, p); //closest point
			float weight = std::max(0.f, bone.radius - (closest - p).norm()) / bone.radius; //weight is influenced by distance to the bone
			new_weights.emplace_back(bone_idx, weight);
			weight_sum += weight; // accumulate the weight
		}
		if (weight_sum < EPS_F) {
			continue; // if no weights, skip this vertex
		}
		for (auto &[bone_idx, weight] : new_weights) {
			if (weight <= EPS_F) continue;
			weight /= weight_sum;
			v.bone_weights.emplace_back(Halfedge_Mesh::Vertex::Bone_Weight{bone_idx, weight}); // assign the normalized weight
		}
	}

}

Indexed_Mesh Skeleton::skin(Halfedge_Mesh const &mesh, std::vector< Mat4 > const &bind, std::vector< Mat4 > const &current) {
	assert(bind.size() == current.size());


	//A4T3: linear blend skinning

	//one approach you might take is to first compute the skinned positions (at every vertex) and normals (at every corner)
	// then generate faces in the style of Indexed_Mesh::from_halfedge_mesh

	//---- step 1: figure out skinned positions ---

	std::unordered_map< Halfedge_Mesh::VertexCRef, Vec3 > skinned_positions;
	std::unordered_map< Halfedge_Mesh::HalfedgeCRef, Vec3 > skinned_normals;
	//reserve hash table space to (one hopes) avoid re-hashing:
	skinned_positions.reserve(mesh.vertices.size());
	skinned_normals.reserve(mesh.halfedges.size());

	//(you will probably want to precompute some bind-to-current transformation matrices here)
	for (auto vi = mesh.vertices.begin(); vi != mesh.vertices.end(); ++vi) {
		Mat4 transform = Mat4::Zero;
		for (auto const &bw : vi->bone_weights) {
			transform += bw.weight * current[bw.bone] * Mat4::inverse(bind[bw.bone]);
		}
		//NOTE: vertices with empty bone_weights should remain in place.
		if (vi->bone_weights.size() == 0) {
			skinned_positions.emplace(vi, vi->position);
		} else {
			skinned_positions.emplace(vi, (transform * Vec4(vi->position, 1.0f)).xyz()); //PLACEHOLDER! Replace with code that computes the position of the vertex according to vi->position and vi->bone_weights.
		}

		//circulate corners at this vertex:
		Mat4 L_i = transform;
		L_i[0][3] = .0f; //make sure translation is zero
		L_i[1][3] = .0f; //make sure translation is zero
		L_i[2][3] = .0f; //make sure translation is zero
		Mat4 N_i = Mat4::transpose(Mat4::inverse(L_i));
		auto h = vi->halfedge;
		do {
			//NOTE: could skip if h->face->boundary, since such corners don't get emitted
			if (h->face->boundary) {
				skinned_normals.emplace(h, h->corner_normal);
				continue; //skip boundary faces
			}
			Vec3 new_corner_normal = (N_i * Vec4(h->corner_normal, 0.0f)).xyz().unit(); //transform normal vector
			skinned_normals.emplace(h, new_corner_normal); //PLACEHOLDER! Replace with code that properly transforms the normal vector! Make sure that you normalize correctly.

			h = h->twin->next;
		} while (h != vi->halfedge);
	}

	//---- step 2: transform into an indexed mesh ---

	//Hint: you should be able to use the code from Indexed_Mesh::from_halfedge_mesh (SplitEdges version) pretty much verbatim, you'll just need to fill in the positions and normals.
	std::vector<Indexed_Mesh::Vert> verts;
	std::vector<Indexed_Mesh::Index> idxs;

	for (Halfedge_Mesh::FaceCRef f = mesh.faces.begin(); f != mesh.faces.end(); f++) {
		if (f->boundary) continue;

		//every corner gets its own copy of a vertex:
		uint32_t corners_begin = static_cast<uint32_t>(verts.size());
		Halfedge_Mesh::HalfedgeCRef h = f->halfedge;
		do {
			Indexed_Mesh::Vert vert;
			vert.pos = skinned_positions.at(h->vertex);
			vert.norm = skinned_normals.at(h);
			vert.uv = h->corner_uv;
			vert.id = f->id;
			verts.emplace_back(vert);
			h = h->next;
		} while (h != f->halfedge);
		uint32_t corners_end = static_cast<uint32_t>(verts.size());

		//divide face into a triangle fan:
		for (size_t i = corners_begin + 1; i + 1 < corners_end; i++) {
			idxs.emplace_back(corners_begin);
			idxs.emplace_back(static_cast<uint32_t>(i));
			idxs.emplace_back(static_cast<uint32_t>(i+1));
		}
	}

	return Indexed_Mesh(std::move(verts), std::move(idxs));
}

void Skeleton::for_bones(const std::function<void(Bone&)>& f) {
	for (auto& bone : bones) {
		f(bone);
	}
}


void Skeleton::erase_bone(BoneIndex bone) {
	assert(bone < bones.size());
	//update indices in bones:
	for (uint32_t b = 0; b < bones.size(); ++b) {
		if (bones[b].parent == -1U) continue;
		if (bones[b].parent == bone) {
			assert(b > bone); //topological sort!
			//keep bone tips in the same place when deleting parent bone:
			bones[b].extent += bones[bone].extent;
			bones[b].parent = bones[bone].parent;
		} else if (bones[b].parent > bone) {
			assert(b > bones[b].parent); //topological sort!
			bones[b].parent -= 1;
		}
	}
	// erase the bone
	bones.erase(bones.begin() + bone);
	//update indices in handles (and erase any handles on this bone):
	for (uint32_t h = 0; h < handles.size(); /* later */) {
		if (handles[h].bone == bone) {
			erase_handle(h);
		} else if (handles[h].bone > bone) {
			handles[h].bone -= 1;
			++h;
		} else {
			++h;
		}
	}
}

void Skeleton::erase_handle(HandleIndex handle) {
	assert(handle < handles.size());

	//nothing internally refers to handles by index so can just delete:
	handles.erase(handles.begin() + handle);
}


Skeleton::BoneIndex Skeleton::add_bone(BoneIndex parent, Vec3 extent) {
	assert(parent == -1U || parent < bones.size());
	Bone bone;
	bone.extent = extent;
	bone.parent = parent;
	//all other parameters left as default.

	//slightly unfortunate hack:
	//(to ensure increasing IDs within an editing session, but reset on load)
	std::unordered_set< uint32_t > used;
	for (auto const &b : bones) {
		used.emplace(b.channel_id);
	}
	while (used.count(next_bone_channel_id)) ++next_bone_channel_id;
	bone.channel_id = next_bone_channel_id++;

	//all other parameters left as default.

	BoneIndex index = BoneIndex(bones.size());
	bones.emplace_back(bone);

	return index;
}

Skeleton::HandleIndex Skeleton::add_handle(BoneIndex bone, Vec3 target) {
	assert(bone < bones.size());
	Handle handle;
	handle.bone = bone;
	handle.target = target;
	//all other parameters left as default.

	//slightly unfortunate hack:
	//(to ensure increasing IDs within an editing session, but reset on load)
	std::unordered_set< uint32_t > used;
	for (auto const &h : handles) {
		used.emplace(h.channel_id);
	}
	while (used.count(next_handle_channel_id)) ++next_handle_channel_id;
	handle.channel_id = next_handle_channel_id++;

	HandleIndex index = HandleIndex(handles.size());
	handles.emplace_back(handle);

	return index;
}


Skeleton Skeleton::copy() {
	//turns out that there aren't any fancy pointer data structures to fix up here.
	return *this;
}

void Skeleton::make_valid() {
	for (uint32_t b = 0; b < bones.size(); ++b) {
		if (!(bones[b].parent == -1U || bones[b].parent < b)) {
			warn("bones[%u].parent is %u, which is not < %u; setting to -1.", b, bones[b].parent, b);
			bones[b].parent = -1U;
		}
	}
	if (bones.empty() && !handles.empty()) {
		warn("Have %u handles but no bones. Deleting handles.", uint32_t(handles.size()));
		handles.clear();
	}
	for (uint32_t h = 0; h < handles.size(); ++h) {
		if (handles[h].bone >= HandleIndex(bones.size())) {
			warn("handles[%u].bone is %u, which is not < bones.size(); setting to 0.", h, handles[h].bone);
			handles[h].bone = 0;
		}
	}
}

//-------------------------------------------------

Indexed_Mesh Skinned_Mesh::bind_mesh() const {
	return Indexed_Mesh::from_halfedge_mesh(mesh, Indexed_Mesh::SplitEdges);
}

Indexed_Mesh Skinned_Mesh::posed_mesh() const {
	return Skeleton::skin(mesh, skeleton.bind_pose(), skeleton.current_pose());
}

Skinned_Mesh Skinned_Mesh::copy() {
	return Skinned_Mesh{mesh.copy(), skeleton.copy()};
}
