#include "bvh.h"
#include "aggregate.h"
#include "instance.h"
#include "tri_mesh.h"

#include <stack>

namespace PT {

struct BVHBuildData {
	BVHBuildData(size_t start, size_t range, size_t dst) : start(start), range(range), node(dst) {
	}
	size_t start; ///< start index into the primitive array
	size_t range; ///< range of index into the primitive array
	size_t node;  ///< address to update
};

struct SAHBucketData {
	BBox bb;          ///< bbox of all primitives
	size_t num_prims; ///< number of primitives in the bucket
};

template<typename Primitive>
void BVH<Primitive>::build(std::vector<Primitive>&& prims, size_t max_leaf_size) {
	//A3T3 - build a bvh

	// Keep these
    nodes.clear();
    primitives = std::move(prims);

    // Construct a BVH from the given vector of primitives and maximum leaf
    // size configuration.

	//TODO
	float best_cost = FLT_MAX;
	int best_axis = -1;

	int buckets_num = 12;
	
	std::stack<BVHBuildData> tasks;

	std::vector<Node> tmp_nodes;

	for (int axis = 0; axis < 3; axis++) {
		tmp_nodes.clear();
		float cost = 0.0f;
		tasks.push(BVHBuildData(0, primitives.size(), 0));
		std::sort(primitives.begin(), primitives.end(),
			[axis](auto const &a, auto const &b) {
				return a.bbox().center()[axis] < b.bbox().center()[axis];
			}
		);
		tmp_nodes.push_back(Node());
		
		// CORE
		while (!tasks.empty()) {
			BVHBuildData task = tasks.top();
			tasks.pop();
			size_t start = task.start;
			size_t range = task.range;
			size_t node_idx = task.node;
			tmp_nodes[node_idx].start = start;
			tmp_nodes[node_idx].size = range;

			BBox prim_bound = primitives[start].bbox();
			BBox centroid_bound(Vec3(FLT_MAX), Vec3(FLT_MIN));
			for (size_t prim_idx = start; prim_idx < start + range; prim_idx++) {
				BBox prim_box = primitives[prim_idx].bbox();
				prim_bound.enclose(prim_box);
				centroid_bound.min.x = std::min(prim_box.center().x, centroid_bound.min.x);
				centroid_bound.min.y = std::min(prim_box.center().y, centroid_bound.min.y);
				centroid_bound.min.z = std::min(prim_box.center().z, centroid_bound.min.z);
				centroid_bound.max.x = std::max(prim_box.center().x, centroid_bound.max.x);
				centroid_bound.max.y = std::max(prim_box.center().y, centroid_bound.max.y);
				centroid_bound.max.z = std::max(prim_box.center().z, centroid_bound.max.z);
			}

			if (tmp_nodes[node_idx].size <= max_leaf_size) {
				tmp_nodes[node_idx].l = node_idx;
				tmp_nodes[node_idx].r = node_idx;
				tmp_nodes[node_idx].bbox = prim_bound;
				if (node_idx == root_idx) {
					nodes = tmp_nodes;
					best_axis = axis;
				}
				continue;
			}

			tmp_nodes[node_idx].bbox = prim_bound;


			std::vector<SAHBucketData> buckets;
			for (int b = 0; b < buckets_num; b++) {
				buckets.push_back({BBox(), (size_t)0});
			}

			for (size_t prim_idx = start; prim_idx < start + range; prim_idx++) {
				BBox prim_bbox = primitives[prim_idx].bbox();
				float t = (prim_bbox.center()[axis] - centroid_bound.min[axis]) 
						/ (centroid_bound.max[axis] - centroid_bound.min[axis]);
				int idx = std::clamp(static_cast<int>(std::floor(t * buckets_num)), 0, buckets_num - 1);
				buckets[idx].num_prims += 1;
				buckets[idx].bb.enclose(prim_bbox);
			}

			float best_split_cost = FLT_MAX;
			int best_split = -1;
			// bug todo
			size_t best_l_cnt = 0;
			for (int split_idx = 0; split_idx < buckets_num - 1; split_idx++) {
				size_t l_cnt = 0;
				size_t r_cnt = 0;
				BBox l_box;
				BBox r_box;
				for (int lb = 0; lb <= split_idx; lb++) {
					l_cnt += buckets[lb].num_prims;
					l_box.enclose(buckets[lb].bb);
				}
				for (int rb = split_idx + 1; rb < buckets_num; rb++) {
					r_cnt += buckets[rb].num_prims;
					r_box.enclose(buckets[rb].bb);
				}
				float current_cost = (float) l_cnt * l_box.surface_area() + r_cnt * r_box.surface_area();
				if (current_cost < best_split_cost) {
					best_split_cost = current_cost;
					best_split = split_idx;
					best_l_cnt = l_cnt;
				}
			}
			cost += best_split_cost;

			// Leaf node
			if (best_l_cnt == 0 || range - best_l_cnt == 0) {
				tmp_nodes[node_idx].l = node_idx; 
				tmp_nodes[node_idx].r = node_idx; 
				if (node_idx == root_idx) {
					nodes = tmp_nodes;
					best_axis = axis;
				}
				continue;
			} else {
				// Left leaf
				Node node_left;
				tmp_nodes.push_back(node_left);
				tmp_nodes[node_idx].l = tmp_nodes.size() - 1;
				tasks.push(BVHBuildData(start, best_l_cnt, tmp_nodes[node_idx].l));
				// Right leaf
				Node node_right;
				tmp_nodes.push_back(node_right);
				tmp_nodes[node_idx].r = tmp_nodes.size() - 1;
				tasks.push(BVHBuildData(start + best_l_cnt , range - best_l_cnt, tmp_nodes[node_idx].r));
			}
		}

		if (cost < best_cost) {
			best_cost = cost;
			nodes = tmp_nodes;
			best_axis = axis;
		}
	}
	
	std::sort(primitives.begin(), primitives.end(),
    [best_axis](auto const &a, auto const &b) {
        return a.bbox().center()[best_axis] < b.bbox().center()[best_axis];
    });

}

template<typename Primitive> Trace BVH<Primitive>::hit(const Ray& ray) const {
	//A3T3 - traverse your BVH

    // Implement ray - BVH intersection test. A ray intersects
    // with a BVH aggregate if and only if it intersects a primitive in
    // the BVH that is not an aggregate.

    // The starter code simply iterates through all the primitives.
    // Again, remember you can use hit() on any Primitive value.

	//TODO: replace this code with a more efficient traversal:
	Ray ray_in = ray;
	std::stack<size_t> stack;
	stack.push(root_idx);
    Trace ret;
	if (primitives.size() == 0) {
		ret.hit = false;
		return ret;
	}
	while (!stack.empty()) {
		size_t idx = stack.top();
		const Node &current_node = nodes[idx];
		stack.pop();
		Vec2 dist_bounds = ray_in.dist_bounds;	// Because bbox.hit will change this value, and we don't wanna it goes in this way.
        if (!current_node.bbox.hit(ray_in, dist_bounds))
            continue;
		if (current_node.is_leaf()) {
			for(size_t prim_idx = current_node.start; prim_idx < current_node.start + current_node.size; prim_idx++) {
				const Primitive& prim = primitives.at(prim_idx);
				Trace hit = prim.hit(ray_in);
				ret = Trace::min(ret, hit);
				if (ret.hit) {
					ray_in.dist_bounds.y = ret.distance;
				}
			}
		} else {
			size_t L = current_node.l, R = current_node.r;
			dist_bounds = ray_in.dist_bounds;
			if (nodes[L].bbox.hit(ray_in, dist_bounds)) {
				stack.push(L);
			}
			dist_bounds = ray_in.dist_bounds;
			if (nodes[R].bbox.hit(ray_in, dist_bounds)) {
				stack.push(R);
			}
		}
	}
    return ret;
}

template<typename Primitive>
BVH<Primitive>::BVH(std::vector<Primitive>&& prims, size_t max_leaf_size) {
	build(std::move(prims), max_leaf_size);
}

template<typename Primitive> std::vector<Primitive> BVH<Primitive>::destructure() {
	nodes.clear();
	return std::move(primitives);
}

template<typename Primitive>
template<typename P>
typename std::enable_if<std::is_copy_assignable_v<P>, BVH<P>>::type BVH<Primitive>::copy() const {
	BVH<Primitive> ret;
	ret.nodes = nodes;
	ret.primitives = primitives;
	ret.root_idx = root_idx;
	return ret;
}

template<typename Primitive> Vec3 BVH<Primitive>::sample(RNG &rng, Vec3 from) const {
	if (primitives.empty()) return {};
	int32_t n = rng.integer(0, static_cast<int32_t>(primitives.size()));
	return primitives[n].sample(rng, from);
}

template<typename Primitive>
float BVH<Primitive>::pdf(Ray ray, const Mat4& T, const Mat4& iT) const {
	if (primitives.empty()) return 0.0f;
	float ret = 0.0f;
	for (auto& prim : primitives) ret += prim.pdf(ray, T, iT);
	return ret / primitives.size();
}

template<typename Primitive> void BVH<Primitive>::clear() {
	nodes.clear();
	primitives.clear();
}

template<typename Primitive> bool BVH<Primitive>::Node::is_leaf() const {
	// A node is a leaf if l == r, since all interior nodes must have distinct children
	return l == r;
}

template<typename Primitive>
size_t BVH<Primitive>::new_node(BBox box, size_t start, size_t size, size_t l, size_t r) {
	Node n;
	n.bbox = box;
	n.start = start;
	n.size = size;
	n.l = l;
	n.r = r;
	nodes.push_back(n);
	return nodes.size() - 1;
}
 
template<typename Primitive> BBox BVH<Primitive>::bbox() const {
	if(nodes.empty()) return BBox{Vec3{0.0f}, Vec3{0.0f}};
	return nodes[root_idx].bbox;
}

template<typename Primitive> size_t BVH<Primitive>::n_primitives() const {
	return primitives.size();
}

template<typename Primitive>
uint32_t BVH<Primitive>::visualize(GL::Lines& lines, GL::Lines& active, uint32_t level,
                                   const Mat4& trans) const {

	std::stack<std::pair<size_t, uint32_t>> tstack;
	tstack.push({root_idx, 0u});
	uint32_t max_level = 0u;

	if (nodes.empty()) return max_level;

	while (!tstack.empty()) {

		auto [idx, lvl] = tstack.top();
		max_level = std::max(max_level, lvl);
		const Node& node = nodes[idx];
		tstack.pop();

		Spectrum color = lvl == level ? Spectrum(1.0f, 0.0f, 0.0f) : Spectrum(1.0f);
		GL::Lines& add = lvl == level ? active : lines;

		BBox box = node.bbox;
		box.transform(trans);
		Vec3 min = box.min, max = box.max;

		auto edge = [&](Vec3 a, Vec3 b) { add.add(a, b, color); };

		edge(min, Vec3{max.x, min.y, min.z});
		edge(min, Vec3{min.x, max.y, min.z});
		edge(min, Vec3{min.x, min.y, max.z});
		edge(max, Vec3{min.x, max.y, max.z});
		edge(max, Vec3{max.x, min.y, max.z});
		edge(max, Vec3{max.x, max.y, min.z});
		edge(Vec3{min.x, max.y, min.z}, Vec3{max.x, max.y, min.z});
		edge(Vec3{min.x, max.y, min.z}, Vec3{min.x, max.y, max.z});
		edge(Vec3{min.x, min.y, max.z}, Vec3{max.x, min.y, max.z});
		edge(Vec3{min.x, min.y, max.z}, Vec3{min.x, max.y, max.z});
		edge(Vec3{max.x, min.y, min.z}, Vec3{max.x, max.y, min.z});
		edge(Vec3{max.x, min.y, min.z}, Vec3{max.x, min.y, max.z});

		if (!node.is_leaf()) {
			tstack.push({node.l, lvl + 1});
			tstack.push({node.r, lvl + 1});
		} else {
			for (size_t i = node.start; i < node.start + node.size; i++) {
				uint32_t c = primitives[i].visualize(lines, active, level - lvl, trans);
				max_level = std::max(c + lvl, max_level);
			}
		}
	}
	return max_level;
}

template class BVH<Triangle>;
template class BVH<Instance>;
template class BVH<Aggregate>;
template BVH<Triangle> BVH<Triangle>::copy<Triangle>() const;

} // namespace PT
