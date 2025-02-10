
#include "transform.h"
#include <iostream>
#include <functional>
#include "test.h"

Mat4 Transform::local_to_parent() const {
	return Mat4::translate(translation) * rotation.to_mat() * Mat4::scale(scale);
}

Mat4 Transform::parent_to_local() const {
	return Mat4::scale(1.0f / scale) * rotation.inverse().to_mat() * Mat4::translate(-translation);
}


// NOTE
// Convert the local coordinates to world coordinates. All local coordinates are based on the 
// parent (basically, the parent is the original point), so first apply local transform, then
// recursively apply parents' transformation.
Mat4 Transform::local_to_world() const {
	// A1T1: local_to_world
	//don't use Mat4::inverse() in your code.
	
	std::function<Mat4(std::weak_ptr<Transform>)> recursive_parent =
        [&](std::weak_ptr<Transform> parent_ptr) -> Mat4 {
            if (auto parent = parent_ptr.lock()) {
                return recursive_parent(parent->parent) * Mat4::translate(parent->translation) * parent->rotation.to_mat() * Mat4::scale(parent->scale);
            } else {
                return Mat4::I;
            }
        };

	return recursive_parent(parent.lock()) * local_to_parent();
}

Mat4 Transform::world_to_local() const {
	// A1T1: world_to_local
	//don't use Mat4::inverse() in your code.

	std::function<Mat4(std::weak_ptr<Transform>)> recursive_parent =
		[&](std::weak_ptr<Transform> parent_ptr) -> Mat4 {
			if (auto parent = parent_ptr.lock()) {
				auto results = Mat4::scale(1.0f / parent->scale) * Mat4::transpose(parent->rotation.to_mat()) * Mat4::translate(-parent->translation) * recursive_parent(parent->parent);
				return results;
			} else {
				return Mat4::I;
			}
		};
	return parent_to_local() * recursive_parent(parent.lock());
}

bool operator!=(const Transform& a, const Transform& b) {
	return a.parent.lock() != b.parent.lock() || a.translation != b.translation ||
	       a.rotation != b.rotation || a.scale != b.scale;
}
