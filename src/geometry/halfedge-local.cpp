
#include "halfedge.h"

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>

/******************************************************************
*********************** Local Operations **************************
******************************************************************/

/* Note on local operation return types:

    The local operations all return a std::optional<T> type. This is used so that your
    implementation can signify that it cannot perform an operation (i.e., because
    the resulting mesh does not have a valid representation).

    An optional can have two values: std::nullopt, or a value of the type it is
    parameterized on. In this way, it's similar to a pointer, but has two advantages:
    the value it holds need not be allocated elsewhere, and it provides an API that
    forces the user to check if it is null before using the value.

    In your implementation, if you have successfully performed the operation, you can
    simply return the required reference:

            ... collapse the edge ...
            return collapsed_vertex_ref;

    And if you wish to deny the operation, you can return the null optional:

            return std::nullopt;

    Note that the stubs below all reject their duties by returning the null optional.
*/


/*
 * add_face: add a standalone face to the mesh
 *  sides: number of sides
 *  radius: distance from vertices to origin
 *
 * We provide this method as an example of how to make new halfedge mesh geometry.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::add_face(uint32_t sides, float radius) {
	//faces with fewer than three sides are invalid, so abort the operation:
	if (sides < 3) return std::nullopt;


	std::vector< VertexRef > face_vertices;
	//In order to make the first edge point in the +x direction, first vertex should
	// be at -90.0f - 0.5f * 360.0f / float(sides) degrees, so:
	float const start_angle = (-0.25f - 0.5f / float(sides)) * 2.0f * PI_F;
	for (uint32_t s = 0; s < sides; ++s) {
		float angle = float(s) / float(sides) * 2.0f * PI_F + start_angle;
		VertexRef v = emplace_vertex();
		v->position = radius * Vec3(std::cos(angle), std::sin(angle), 0.0f);
		face_vertices.emplace_back(v);
	}

	assert(face_vertices.size() == sides);

	//assemble the rest of the mesh parts:
	FaceRef face = emplace_face(false); //the face to return
	FaceRef boundary = emplace_face(true); //the boundary loop around the face

	std::vector< HalfedgeRef > face_halfedges; //will use later to set ->next pointers

	for (uint32_t s = 0; s < sides; ++s) {
		//will create elements for edge from a->b:
		VertexRef a = face_vertices[s];
		VertexRef b = face_vertices[(s+1)%sides];

		//h is the edge on face:
		HalfedgeRef h = emplace_halfedge();
		//t is the twin, lies on boundary:
		HalfedgeRef t = emplace_halfedge();
		//e is the edge corresponding to h,t:
		EdgeRef e = emplace_edge(false); //false: non-sharp

		//set element data to something reasonable:
		//(most ops will do this with interpolate_data(), but no data to interpolate here)
		h->corner_uv = a->position.xy() / (2.0f * radius) + 0.5f;
		h->corner_normal = Vec3(0.0f, 0.0f, 1.0f);
		t->corner_uv = b->position.xy() / (2.0f * radius) + 0.5f;
		t->corner_normal = Vec3(0.0f, 0.0f,-1.0f);

		//thing -> halfedge pointers:
		e->halfedge = h;
		a->halfedge = h;
		if (s == 0) face->halfedge = h;
		if (s + 1 == sides) boundary->halfedge = t;

		//halfedge -> thing pointers (except 'next' -- will set that later)
		h->twin = t;
		h->vertex = a;
		h->edge = e;
		h->face = face;

		t->twin = h;
		t->vertex = b;
		t->edge = e;
		t->face = boundary;

		face_halfedges.emplace_back(h);
	}

	assert(face_halfedges.size() == sides);

	/**
	 * When add a independent face, there isn't any adjacent face,
	 * So we "fake" a boundary face to store all the twin halfedge.
	 * We can image a virtual boundary face around the original face.
	 */
	for (uint32_t s = 0; s < sides; ++s) {
		face_halfedges[s]->next = face_halfedges[(s+1)%sides];
		face_halfedges[(s+1)%sides]->twin->next = face_halfedges[s]->twin;
	}

	return face;
}


/*
 * bisect_edge: split an edge without splitting the adjacent faces
 *  e: edge to split
 *
 * returns: added vertex
 *
 * We provide this as an example for how to implement local operations.
 * (and as a useful subroutine!)
 */
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::bisect_edge(EdgeRef e) {
	// Phase 0: draw a picture
	//
	// before:
	//    ----h--->
	// v1 ----e--- v2
	//   <----t---
	//
	// after:
	//    --h->    --h2->
	// v1 --e-- vm --e2-- v2
	//    <-t2-    <--t--
	//

	// Phase 1: collect existing elements
	HalfedgeRef h = e->halfedge;
	HalfedgeRef t = h->twin;
	VertexRef v1 = h->vertex;
	VertexRef v2 = t->vertex;

	// Phase 2: Allocate new elements, set data
	VertexRef vm = emplace_vertex();
	vm->position = (v1->position + v2->position) / 2.0f;
	interpolate_data({v1, v2}, vm); //set bone_weights

	EdgeRef e2 = emplace_edge();
	e2->sharp = e->sharp; //copy sharpness flag

	HalfedgeRef h2 = emplace_halfedge();
	interpolate_data({h, h->next}, h2); //set corner_uv, corner_normal

	HalfedgeRef t2 = emplace_halfedge();
	interpolate_data({t, t->next}, t2); //set corner_uv, corner_normal

	// The following elements aren't necessary for the bisect_edge, but they are here to demonstrate phase 4
    FaceRef f_not_used = emplace_face();
    HalfedgeRef h_not_used = emplace_halfedge();

	// Phase 3: Reassign connectivity (careful about ordering so you don't overwrite values you may need later!)

	vm->halfedge = h2;

	e2->halfedge = h2;

	assert(e->halfedge == h); //unchanged

	//n.b. h remains on the same face so even if h->face->halfedge == h, no fixup needed (t, similarly)

	h2->twin = t;
	h2->next = h->next;
	h2->vertex = vm;
	h2->edge = e2;
	h2->face = h->face;

	t2->twin = h;
	t2->next = t->next;
	t2->vertex = vm;
	t2->edge = e;
	t2->face = t->face;
	
	h->twin = t2;
	h->next = h2;
	assert(h->vertex == v1); // unchanged
	assert(h->edge == e); // unchanged
	//h->face unchanged

	t->twin = h2;
	t->next = t2;
	assert(t->vertex == v2); // unchanged
	t->edge = e2;
	//t->face unchanged


	// Phase 4: Delete unused elements
    erase_face(f_not_used);
    erase_halfedge(h_not_used);

	// Phase 5: Return the correct iterator
	return vm;
}


/*
 * split_edge: split an edge and adjacent (non-boundary) faces
 *  e: edge to split
 *
 * returns: added vertex. vertex->halfedge should lie along e
 *
 * Note that when splitting the adjacent faces, the new edge
 * should connect to the vertex ccw from the ccw-most end of e
 * within the face.
 *
 * Do not split adjacent boundary faces.
 */
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::split_edge(EdgeRef e) {
	// A2L2 (REQUIRED): split_edge
	// Get the new vertex
	// Split the edge e
	std::optional<VertexRef> optional_v = bisect_edge(e); 
	assert(optional_v.has_value());
	VertexRef v = optional_v.value();
	// Deal with the non-boundary case first
	if (!(e->halfedge->face->boundary || e->halfedge->twin->face->boundary)) {
		// Pre-store the halfedge
		HalfedgeRef h = v->halfedge->twin->next->twin;
		HalfedgeRef t = v->halfedge->twin;
		HalfedgeRef h_next = h->next->next;
		HalfedgeRef h_next2 = h->next->next->next;
		HalfedgeRef t_next = t->next->next;
		HalfedgeRef t_next2 = t->next->next->next;

		// Get all the new elements
		// h means halfedge side, t means twin side
		EdgeRef edge_h = emplace_edge();
		HalfedgeRef h_e_h = emplace_halfedge();
		HalfedgeRef t_e_h = emplace_halfedge();
		EdgeRef edge_t= emplace_edge();
		HalfedgeRef h_e_t = emplace_halfedge();
		HalfedgeRef t_e_t = emplace_halfedge();
		FaceRef face_h = emplace_face();
		FaceRef face_t = emplace_face();

		// First: assemable the new halfedges, edges
		edge_h->sharp = e->sharp;
		edge_t->sharp = e->sharp;
		h_e_h->twin = t_e_h; // set twin
		t_e_h->twin = h_e_h;
		h_e_t->twin = t_e_t;
		t_e_t->twin = h_e_t;
		h_e_h->vertex = v; // set startpoint
		t_e_h->vertex = h_next2->vertex;
		h_e_t->vertex = v;
		t_e_t->vertex = t_next2->vertex;
		edge_h->halfedge = h_e_h; // connect halfedge and edge
		edge_t->halfedge = h_e_t;
		h_e_h->edge = edge_h;
		t_e_h->edge = edge_h;
		h_e_t->edge = edge_t;
		t_e_t->edge = edge_t;

		// Second: set all changed halfedges' next
		h_e_h->next = h_next2; // set new halfedges' next
		t_e_h->next = h->next;
		h_e_t->next = t_next2;
		t_e_t->next = h->twin;
		h->next->next->next = t_e_h; // set affected halfedges' next
		t->next->next->next = t_e_t;
		h->next = h_e_h;
		t->next = h_e_t;

		// Third: set face for all effected halfedges
		face_h->boundary = false;
		face_t->boundary = false;
		face_h->halfedge = t_e_h;
		face_t->halfedge = t_e_t;
		HalfedgeRef hfedge_h = t_e_h; // iteratively set face for new halfedges for h side;
		while (hfedge_h->next != t_e_h) {
			hfedge_h->face = face_h;
			hfedge_h = hfedge_h->next;
		}
		hfedge_h->face = face_h;
		HalfedgeRef hfedge_t = t_e_t; // iteratively set face for new halfedges for t side;
		while (hfedge_t->next != t_e_t) {
			hfedge_t->face = face_t;
			hfedge_t = hfedge_t->next;
		}
		hfedge_t->face = face_t;
		h_e_h->face = h->face;
		h_e_t->face = t->face;
		h->face->halfedge = h; 
		t->face->halfedge = t; 

		// Forth: set new data
		interpolate_data({h, h_e_h->next}, h_e_h);
		interpolate_data({hfedge_h, t_e_h->next}, t_e_h);
		interpolate_data({t, h_e_t->next}, h_e_t);
		interpolate_data({hfedge_t, t_e_t->next}, t_e_t);
		return v;
	} else {
		// Pre-store the halfedge
		HalfedgeRef h = v->halfedge->face->boundary ? v->halfedge->next->twin : v->halfedge->twin->next->twin;
		HalfedgeRef h_next = h->next->next;
		HalfedgeRef h_next2 = h->next->next->next;

		// Get all the new elements
		// h means halfedge side, t means twin side
		EdgeRef edge_h = emplace_edge();
		HalfedgeRef h_e_h = emplace_halfedge();
		HalfedgeRef t_e_h = emplace_halfedge();
		FaceRef face_h = emplace_face();

		// First: assemable the new halfedges, edges
		edge_h->sharp = e->sharp;
		h_e_h->twin = t_e_h; // set twin
		t_e_h->twin = h_e_h;
		h_e_h->vertex = v; // set startpoint
		t_e_h->vertex = h_next2->vertex;
		edge_h->halfedge = h_e_h; // connect halfedge and edge
		h_e_h->edge = edge_h;
		t_e_h->edge = edge_h;
		
		// Second: set all changed halfedges' next
		h_e_h->next = h_next2; // set new halfedges' next
		t_e_h->next = h->next;
		h->next->next->next = t_e_h; // set affected halfedges' next
		h->next = h_e_h;

		// Third: set face for all effected halfedges
		face_h->boundary = false;
		face_h->halfedge = t_e_h;
		HalfedgeRef hfedge_h = t_e_h; // iteratively set face for new halfedges for h side;
		while (hfedge_h->next != t_e_h) {
			hfedge_h->face = face_h;
			hfedge_h = hfedge_h->next;
		}
		hfedge_h->face = face_h;
		h_e_h->face = h->face;
		h->face->halfedge = h; 

		// Forth: set new data
		interpolate_data({h, h_e_h->next}, h_e_h);
		interpolate_data({hfedge_h, t_e_h->next}, t_e_h);
		return v;
	}
}



/*
 * inset_vertex: divide a face into triangles by placing a vertex at f->center()
 *  f: the face to add the vertex to
 *
 * returns:
 *  std::nullopt if insetting a vertex would make mesh invalid
 *  the inset vertex otherwise
 */
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::inset_vertex(FaceRef f) {
	// A2Lx4 (OPTIONAL): inset vertex
	
	(void)f;
    return std::nullopt;
}


/* [BEVEL NOTE] Note on the beveling process:

	Each of the bevel_vertex, bevel_edge, and extrude_face functions do not represent
	a full bevel/extrude operation. Instead, they should update the _connectivity_ of
	the mesh, _not_ the positions of newly created vertices. In fact, you should set
	the positions of new vertices to be exactly the same as wherever they "started from."

	When you click on a mesh element while in bevel mode, one of those three functions
	is called. But, because you may then adjust the distance/offset of the newly
	beveled face, we need another method of updating the positions of the new vertices.

	This is where bevel_positions and extrude_positions come in: these functions are
	called repeatedly as you move your mouse, the position of which determines the
	amount / shrink parameters. These functions are also passed an array of the original
	vertex positions, stored just after the bevel/extrude call, in order starting at
	face->halfedge->vertex, and the original element normal, computed just *before* the
	bevel/extrude call.

	Finally, note that the amount, extrude, and/or shrink parameters are not relative
	values -- you should compute a particular new position from them, not a delta to
	apply.
*/

/*
 * bevel_vertex: creates a face in place of a vertex
 *  v: the vertex to bevel
 *
 * returns: reference to the new face
 *
 * see also [BEVEL NOTE] above.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_vertex(VertexRef v) {
	//A2Lx5 (OPTIONAL): Bevel Vertex
	// Reminder: This function does not update the vertex positions.
	// Remember to also fill in bevel_vertex_helper (A2Lx5h)

	(void)v;
    return std::nullopt;
}

/*
 * bevel_edge: creates a face in place of an edge
 *  e: the edge to bevel
 *
 * returns: reference to the new face
 *
 * see also [BEVEL NOTE] above.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::bevel_edge(EdgeRef e) {
	//A2Lx6 (OPTIONAL): Bevel Edge
	// Reminder: This function does not update the vertex positions.
	// remember to also fill in bevel_edge_helper (A2Lx6h)

	(void)e;
    return std::nullopt;
}

/*
 * extrude_face: creates a face inset into a face
 *  f: the face to inset
 *
 * returns: reference to the inner face
 *
 * see also [BEVEL NOTE] above.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::extrude_face(FaceRef f) {
	//A2L4: Extrude Face
	// Reminder: This function does not update the vertex positions.
	// Remember to also fill in extrude_helper (A2L4h)

	(void)f;
    return std::nullopt;
}

/*
 * flip_edge: rotate non-boundary edge ccw inside its containing faces
 *  e: edge to flip
 *
 * if e is a boundary edge, does nothing and returns std::nullopt
 * if flipping e would create an invalid mesh, does nothing and returns std::nullopt
 *
 * otherwise returns the edge, post-rotation
 *
 * does not create or destroy mesh elements.
 */
std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::flip_edge(EdgeRef e) {	
	//A2L1: Flip Edge
	if (e->on_boundary()) {
		return std::nullopt;
	}

	HalfedgeRef& h = e->halfedge;
	HalfedgeRef& t = h->twin;
	
	// update the vertex's halfedge
	h->vertex->halfedge = h->vertex->halfedge->edge == e ? t->next : h->vertex->halfedge;
	t->vertex->halfedge = t->vertex->halfedge->edge == e ? h->next : t->vertex->halfedge;
	
	// update the new start vertex
	h->vertex = t->next->next->vertex;
	t->vertex = h->next->next->vertex;
	
	// define template variable
	HalfedgeRef h_next = h->next;
	HalfedgeRef t_next = t->next;
	HalfedgeRef h_next2 = h->next->next;
	HalfedgeRef t_next2 = t->next->next;
	HalfedgeRef h_last = h;
	while (h_last->next != h) h_last = h_last->next;

	// update the face and nextabout the changed vertex
	h_next->face = t->face;
	t_next->face = h->face;
	h_next->next = t;
	t_next->next = h;
	h_last->next = t_next;
	
	// update the h and t
	h->next = h_next2;
	t->next = t_next2;
	t->next->next = h_next;
	return e;
}


/*
 * make_boundary: add non-boundary face to boundary
 *  face: the face to make part of the boundary
 *
 * if face ends up adjacent to other boundary faces, merge them into face
 *
 * if resulting mesh would be invalid, does nothing and returns std::nullopt
 * otherwise returns face
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::make_boundary(FaceRef face) {
	//A2Lx7: (OPTIONAL) make_boundary

	return std::nullopt; //TODO: actually write this code!
}

/*
 * dissolve_vertex: merge non-boundary faces adjacent to vertex, removing vertex
 *  v: vertex to merge around
 *
 * if merging would result in an invalid mesh, does nothing and returns std::nullopt
 * otherwise returns the merged face
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::dissolve_vertex(VertexRef v) {
	// A2Lx1 (OPTIONAL): Dissolve Vertex

    return std::nullopt;
}

/*
 * dissolve_edge: merge the two faces on either side of an edge
 *  e: the edge to dissolve
 *
 * merging a boundary and non-boundary face produces a boundary face.
 *
 * if the result of the merge would be an invalid mesh, does nothing and returns std::nullopt
 * otherwise returns the merged face.
 */
std::optional<Halfedge_Mesh::FaceRef> Halfedge_Mesh::dissolve_edge(EdgeRef e) {
	// A2Lx2 (OPTIONAL): dissolve_edge

	//Reminder: use interpolate_data() to merge corner_uv / corner_normal data
	
    return std::nullopt;
}

/* collapse_edge: collapse edge to a vertex at its middle
 *  e: the edge to collapse
 *
 * if collapsing the edge would result in an invalid mesh, does nothing and returns std::nullopt
 * otherwise returns the newly collapsed vertex
 */
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_edge(EdgeRef e) {
	//A2L3: Collapse Edge

	//Reminder: use interpolate_data() to merge corner_uv / corner_normal data on halfedges
	// (also works for bone_weights data on vertices!)
	
	
	HalfedgeRef h = e->halfedge;
	HalfedgeRef t = e->halfedge->twin;
	HalfedgeRef tmp_hf;


	// Deal each situation
	
	/**
	 * First: Rejection
	 * If the collapsed edge is contained within any isolated triangle,
	 * we rejected this opertion, because it will generate a single line 
	 */
	// 
	VertexRef v_h = h->vertex;
	VertexRef v_t = t->vertex;
	v_h->halfedge = h;
	v_t->halfedge = t;
	auto get_vertex_edge_number = [](VertexRef v) { 
		// only for vertex whose halfedge is not halfedges.end()
		int answer = 0;
		HalfedgeRef temp = v->halfedge;
		do {
			answer++;
			temp = temp->twin->next;
		} while (temp->twin->next != v->halfedge);
		return answer;
	};
	
	// TODO
	// // h's start point
	// tmp_hf = v_h->halfedge;
	// do {
	// 	if (tmp_hf->next->next->next->vertex == v_h 
	// 		&& get_vertex_edge_number(tmp_hf->next->next->vertex) == 2) {
	// 			return std::nullopt;
	// 	}
	// 	tmp_hf = tmp_hf->twin->next;
	// } while (tmp_hf != v_h->halfedge);
		
	// // t's start point
	// tmp_hf = v_t->halfedge;
	// do {
	// 	if (tmp_hf->next->next->next->vertex == v_h 
	// 		&& get_vertex_edge_number(tmp_hf->next->next->vertex) == 2) {
	// 			return std::nullopt;
	// 		}
	// 	tmp_hf = tmp_hf->twin->next;
	// } while (tmp_hf != v_t->halfedge);


	/**
	 * Second: Create new vertex
	 * Create the new vertex, including interpolating data
	 */
	VertexRef v = emplace_vertex();
	interpolate_data({v_h, v_t}, v);
	v->position = (v_h->position + v_t->position) / 2.0f;


	/**
	 * Third: Connectivity
	 * Here connect the old relative vertex to the new one
	 */
	std::vector<std::pair<HalfedgeRef, HalfedgeRef>> from_to_vh;
	std::vector<std::pair<HalfedgeRef, HalfedgeRef>> from_to_vt;


	// h's start point
	tmp_hf = v_h->halfedge;
	do{
		if (tmp_hf != h && tmp_hf->twin != t) {
			from_to_vh.push_back({tmp_hf, tmp_hf->twin});
		}	
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != v_h->halfedge);
	
	// t's start point
	tmp_hf = v_t->halfedge;
	do{
		if (tmp_hf != t && tmp_hf->twin != h) {
			from_to_vt.push_back({tmp_hf, tmp_hf->twin});
		}
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != v_t->halfedge);

	// vertex h's connectivity
	for (uint32_t i = 0; i < from_to_vh.size(); i++) {
		HalfedgeRef from_vh = from_to_vh[i].first;
		HalfedgeRef to_vh = from_to_vh[i].second;
		
		from_vh->vertex = v;
		if (to_vh->next == h) {
			to_vh->next = h->next;
			v->halfedge = from_vh;
		}
	}
	
	// vertex t's connectivity 
	for (uint32_t i = 0; i < from_to_vt.size(); i++) {
		HalfedgeRef from_vt = from_to_vt[i].first;
		HalfedgeRef to_vt = from_to_vt[i].second;

		from_vt->vertex = v;
		if (to_vt->next == t) {
			to_vt->next = t->next;
		}
	}

	/**
	 * Forth: delete the origianle e and triangle (if exists) 
	 */
	std::vector<VertexRef> wait_to_erase_vertex;
	std::vector<EdgeRef> wait_to_erase_edge;
	std::vector<FaceRef> wait_to_erase_face;
	std::vector<HalfedgeRef> wait_to_erase_halfedge;
	wait_to_erase_edge.push_back(e);
	wait_to_erase_vertex.push_back(v_h);
	wait_to_erase_vertex.push_back(v_t);
	h->face->halfedge = h->face->halfedge->next;
	t->face->halfedge = t->face->halfedge->next;
	wait_to_erase_halfedge.push_back(h);
	wait_to_erase_halfedge.push_back(t);
	tmp_hf = v->halfedge;
	do {
		if (tmp_hf->next->next == tmp_hf) {
			tmp_hf->twin->twin = tmp_hf->next->twin;
			tmp_hf->next->twin->twin = tmp_hf->twin;
			tmp_hf->next->twin->edge = tmp_hf->twin->edge;
			tmp_hf->twin->edge->halfedge = tmp_hf->twin;
			tmp_hf->next->vertex->halfedge = tmp_hf->twin;
			wait_to_erase_halfedge.push_back(tmp_hf);
			wait_to_erase_halfedge.push_back(tmp_hf->next);
			wait_to_erase_face.push_back(tmp_hf->face);
			wait_to_erase_edge.push_back(tmp_hf->next->edge);
		}
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != v->halfedge);
	
	for (auto x : wait_to_erase_vertex) erase_vertex(x);
	for (auto x : wait_to_erase_edge) erase_edge(x);
	for (auto x : wait_to_erase_face) erase_face(x);
	for (auto x : wait_to_erase_halfedge) erase_halfedge(x);

	return v;
}

/*
 * collapse_face: collapse a face to a single vertex at its center
 *  f: the face to collapse
 *
 * if collapsing the face would result in an invalid mesh, does nothing and returns std::nullopt
 * otherwise returns the newly collapsed vertex
 */
std::optional<Halfedge_Mesh::VertexRef> Halfedge_Mesh::collapse_face(FaceRef f) {
	//A2Lx3 (OPTIONAL): Collapse Face

	//Reminder: use interpolate_data() to merge corner_uv / corner_normal data on halfedges
	// (also works for bone_weights data on vertices!)

    return std::nullopt;
}

/*
 * weld_edges: glue two boundary edges together to make one non-boundary edge
 *  e, e2: the edges to weld
 *
 * if welding the edges would result in an invalid mesh, does nothing and returns std::nullopt
 * otherwise returns e, updated to represent the newly-welded edge
 */
std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::weld_edges(EdgeRef e, EdgeRef e2) {
	//A2Lx8: Weld Edges

	//Reminder: use interpolate_data() to merge bone_weights data on vertices!

    return std::nullopt;
}



/*
 * bevel_positions: compute new positions for the vertices of a beveled vertex/edge
 *  face: the face that was created by the bevel operation
 *  start_positions: the starting positions of the vertices
 *     start_positions[i] is the starting position of face->halfedge(->next)^i
 *  direction: direction to bevel in (unit vector)
 *  distance: how far to bevel
 *
 * push each vertex from its starting position along its outgoing edge until it has
 *  moved distance `distance` in direction `direction`. If it runs out of edge to
 *  move along, you may choose to extrapolate, clamp the distance, or do something
 *  else reasonable.
 *
 * only changes vertex positions (no connectivity changes!)
 *
 * This is called repeatedly as the user interacts, just after bevel_vertex or bevel_edge.
 * (So you can assume the local topology is set up however your bevel_* functions do it.)
 *
 * see also [BEVEL NOTE] above.
 */
void Halfedge_Mesh::bevel_positions(FaceRef face, std::vector<Vec3> const &start_positions, Vec3 direction, float distance) {
	//A2Lx5h / A2Lx6h (OPTIONAL): Bevel Positions Helper
	
	// The basic strategy here is to loop over the list of outgoing halfedges,
	// and use the preceding and next vertex position from the original mesh
	// (in the start_positions array) to compute an new vertex position.
	
}

/*
 * extrude_positions: compute new positions for the vertices of an extruded face
 *  face: the face that was created by the extrude operation
 *  move: how much to translate the face
 *  shrink: amount to linearly interpolate vertices in the face toward the face's centroid
 *    shrink of zero leaves the face where it is
 *    positive shrink makes the face smaller (at shrink of 1, face is a point)
 *    negative shrink makes the face larger
 *
 * only changes vertex positions (no connectivity changes!)
 *
 * This is called repeatedly as the user interacts, just after extrude_face.
 * (So you can assume the local topology is set up however your extrude_face function does it.)
 *
 * Using extrude face in the GUI will assume a shrink of 0 to only extrude the selected face
 * Using bevel face in the GUI will allow you to shrink and increase the size of the selected face
 * 
 * see also [BEVEL NOTE] above.
 */
void Halfedge_Mesh::extrude_positions(FaceRef face, Vec3 move, float shrink) {
	//A2L4h: Extrude Positions Helper

	//General strategy:
	// use mesh navigation to get starting positions from the surrounding faces,
	// compute the centroid from these positions + use to shrink,
	// offset by move
	
}

