
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
	if (f->boundary) {
		return std::nullopt;
	}
	
	VertexRef new_vertex = emplace_vertex();
	
	std::vector<HalfedgeRef> new_halfedges;
	std::vector<HalfedgeRef> hf_adjust;
	
	HalfedgeRef tmp_hf = f->halfedge;
	do {
		hf_adjust.push_back(tmp_hf);
		new_halfedges.push_back(emplace_halfedge());
		new_halfedges.push_back(emplace_halfedge());
		tmp_hf = tmp_hf -> next;
	} while (tmp_hf != f->halfedge);
	
	std::vector<VertexCRef> const_vertices;
	for (int i = 0; i < hf_adjust.size(); i++) {
		const_vertices.push_back(hf_adjust[i]->vertex);
		new_vertex->position += hf_adjust[i]->vertex->position / (float) hf_adjust.size();
	}
	interpolate_data(const_vertices, new_vertex);
	new_vertex->halfedge = new_halfedges[1];

	for (int i = 0; i < hf_adjust.size(); i++) {
		hf_adjust[i]->next = new_halfedges[2 * i];
		new_halfedges[2 * i]->next = new_halfedges[2 * i + 1];
		new_halfedges[2 * i + 1]->next = hf_adjust[i];

		new_halfedges[2 * i]->vertex = hf_adjust[i]->twin->vertex;
		new_halfedges[2 * i + 1]->vertex = new_vertex;

		FaceRef new_face;
		if (i == 0) {
			new_face = hf_adjust[i]->face;
		} else {
			new_face = emplace_face();
		}
		hf_adjust[i]->face = new_face;
		new_halfedges[2 * i]->face = new_face;
		new_halfedges[2 * i + 1]->face = new_face;
		
		new_halfedges[2 * i]->twin = new_halfedges[2 * ((i + 1) % hf_adjust.size()) + 1];
		new_halfedges[2 * i + 1]->twin = new_halfedges[2 * ((i - 1 + hf_adjust.size()) % hf_adjust.size())];

		EdgeRef new_edge = emplace_edge();
		new_halfedges[2 * i]->edge = new_edge;
		new_halfedges[2 * ((i + 1) % hf_adjust.size()) + 1]->edge = new_edge;
		new_edge->halfedge = new_halfedges[2 * i];

		new_face->halfedge = hf_adjust[i];
	}

	return new_vertex;
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

	FaceRef new_face = emplace_face();
	std::vector<VertexRef> new_vertices;
	std::vector<EdgeRef> new_edges;
	std::vector<HalfedgeRef> new_halfedges;
	std::vector<HalfedgeRef> adjust_halfedges;
	HalfedgeRef tmp_hf = v->halfedge;
	do {
		adjust_halfedges.push_back(tmp_hf);
		adjust_halfedges.push_back(tmp_hf->twin);
		new_halfedges.push_back(emplace_halfedge());
		new_halfedges.push_back(emplace_halfedge());
		new_edges.push_back(emplace_edge());
		new_vertices.push_back(emplace_vertex());
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != v->halfedge);

	size_t num = new_vertices.size();

	for (int i = 0; i < num; i++) {
		adjust_halfedges[2 * i]->vertex = new_vertices[i];
		adjust_halfedges[2 * i + 1]->next = new_halfedges[2 * i];

		new_halfedges[2 * i]->face = adjust_halfedges[2 * i + 1]->face;
		new_halfedges[2 * i]->vertex = new_vertices[i];
		new_halfedges[2 * i]->edge = new_edges[i];
		new_halfedges[2 * i]->twin = new_halfedges[2 * i + 1];
		new_halfedges[2 * i]->next = adjust_halfedges[2 * ((i + 1) % num)];

		new_halfedges[2 * i + 1]->face = new_face;
		new_halfedges[2 * i + 1]->vertex = new_vertices[(i + 1) % num];
		new_halfedges[2 * i + 1]->edge = new_edges[i];
		new_halfedges[2 * i + 1]->twin = new_halfedges[2 * i];
		new_halfedges[2 * i + 1]->next = new_halfedges[2 * ((i - 1 + num) % num) + 1];

		new_edges[i]->halfedge = new_halfedges[2 * i];

		new_vertices[i]->halfedge = new_halfedges[2 * i];
		new_vertices[i]->position = v->position;
		interpolate_data({v}, new_vertices[i]);
	}
	new_face->halfedge = new_halfedges[1];

	erase_vertex(v);
    return new_face;
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

	if (e->on_boundary()) {
		return std::nullopt;
	}
	
	FaceRef new_face = emplace_face();
	HalfedgeRef tmp_hf;

	HalfedgeRef h = e->halfedge;
	HalfedgeRef t = h->twin;
	HalfedgeRef h_twin = emplace_halfedge();
	HalfedgeRef t_twin = emplace_halfedge();
	h->twin = h_twin; h_twin->twin = h; 
	t->twin = t_twin; t_twin->twin = t;
	EdgeRef h_edge = emplace_edge();
	EdgeRef t_edge = emplace_edge();
	h_edge->halfedge = h; t_edge->halfedge = t;
	h->edge = h_edge; h_twin->edge = h_edge;
	t->edge = t_edge; t_twin->edge = t_edge;
	Vec3 t_position = h->vertex->position;
	Vec3 h_position = t->vertex->position;
	erase_vertex(h->vertex);
	erase_vertex(t->vertex);
	erase_edge(e);


	// First: deal with the `t` side
	std::vector<HalfedgeRef> t_adjust_halfedges;
	std::vector<HalfedgeRef> t_new_halfedges;
	std::vector<VertexRef> t_new_vertices;
	std::vector<EdgeRef> t_new_edges;

	tmp_hf = t->next;
	do {
		t_adjust_halfedges.push_back(tmp_hf);
		t_adjust_halfedges.push_back(tmp_hf->twin);
		VertexRef v = emplace_vertex();
		v->position = t_position;
		interpolate_data({tmp_hf->vertex, tmp_hf->vertex}, v);
		t_new_vertices.push_back(v);
		if (tmp_hf->twin->next != h) {
			t_new_edges.push_back(emplace_edge());
			t_new_halfedges.push_back(emplace_halfedge());
			t_new_halfedges.push_back(emplace_halfedge());
		}
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != h);

	int t_num = (int) t_new_vertices.size();

	for (int i = 0; i < t_num; i++) {
		t_adjust_halfedges[2 * i]->vertex = t_new_vertices[i];
		t_new_vertices[i]->halfedge = t_adjust_halfedges[2 * i];
		if (i != t_num - 1) {
			t_adjust_halfedges[2 * i + 1]->next = t_new_halfedges[2 * i];
			t_new_halfedges[2 * i]->twin = t_new_halfedges[2 * i + 1];
			t_new_halfedges[2 * i + 1]->twin = t_new_halfedges[2 * i];
			t_new_halfedges[2 * i]->vertex = t_new_vertices[i];
			t_new_halfedges[2 * i + 1]->vertex = t_new_vertices[i + 1];
			t_new_halfedges[2 * i]->face = t_adjust_halfedges[2 * i + 1]->face;
			t_new_halfedges[2 * i + 1]->face = new_face;
			t_new_halfedges[2 * i]->next = t_adjust_halfedges[2 * (i + 1)];
			if (i == 0) {
				t_new_halfedges[2 * i + 1]->next = t_twin;
			} else {
				t_new_halfedges[2 * i + 1]->next = t_new_halfedges[2 * (i - 1) + 1];
			}
			t_new_halfedges[2 * i]->edge = t_new_edges[i];
			t_new_halfedges[2 * i + 1]->edge = t_new_edges[i];
			t_new_edges[i]->halfedge = t_new_halfedges[2 * i];
		} else {
			t_adjust_halfedges[2 * i + 1]->next = h;
		}
	}

	
	// Second: deal with the `h` side
	std::vector<HalfedgeRef> h_adjust_halfedges;
	std::vector<HalfedgeRef> h_new_halfedges;
	std::vector<VertexRef> h_new_vertices;
	std::vector<EdgeRef> h_new_edges;

	tmp_hf = h->next;
	do {
		h_adjust_halfedges.push_back(tmp_hf);
		h_adjust_halfedges.push_back(tmp_hf->twin);
		VertexRef v = emplace_vertex();
		v->position = h_position;
		interpolate_data({tmp_hf->vertex, tmp_hf->vertex}, v);
		h_new_vertices.push_back(v);
		if (tmp_hf->twin->next != t) {
			h_new_edges.push_back(emplace_edge());
			h_new_halfedges.push_back(emplace_halfedge());
			h_new_halfedges.push_back(emplace_halfedge());
		}
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != t);

	int h_num = (int) h_new_vertices.size();

	for (int i = 0; i < h_num; i++) {
		h_adjust_halfedges[2 * i]->vertex = h_new_vertices[i];
		h_new_vertices[i]->halfedge = h_adjust_halfedges[2 * i];
		if (i != h_num - 1) {
			h_adjust_halfedges[2 * i + 1]->next = h_new_halfedges[2 * i];
			h_new_halfedges[2 * i]->twin = h_new_halfedges[2 * i + 1];
			h_new_halfedges[2 * i + 1]->twin = h_new_halfedges[2 * i];
			h_new_halfedges[2 * i]->vertex = h_new_vertices[i];
			h_new_halfedges[2 * i + 1]->vertex = h_new_vertices[i + 1];
			h_new_halfedges[2 * i]->face = h_adjust_halfedges[2 * i + 1]->face;
			h_new_halfedges[2 * i + 1]->face = new_face;
			h_new_halfedges[2 * i]->next = h_adjust_halfedges[2 * (i + 1)];
			if (i == 0) {
				h_new_halfedges[2 * i + 1]->next = h_twin;
			} else {
				h_new_halfedges[2 * i + 1]->next = h_new_halfedges[2 * (i - 1) + 1];
			}
			h_new_halfedges[2 * i]->edge = h_new_edges[i];
			h_new_halfedges[2 * i + 1]->edge = h_new_edges[i];
			h_new_edges[i]->halfedge = h_new_halfedges[2 * i];
		} else {
			h_adjust_halfedges[2 * i + 1]->next =  t;
		}
	}

	// Third: deal with the new face
	new_face->halfedge = h_twin;
	h->vertex = t_new_vertices.back();
	h_twin->vertex = h_new_vertices[0];
	t->vertex = h_new_vertices.back();
	t_twin->vertex = t_new_vertices[0];
	h_twin->next = t_new_halfedges.back();
	t_twin->next = h_new_halfedges.back();
	h_twin->face = new_face;
	t_twin->face = new_face;

	return new_face;
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

	HalfedgeRef hf_tmp;
	
	/**
	 * 1. Create x new vertices/faces, 2 * x edges, and 4 * x halfedges
	 */
	
	int v_num = 0;
	std::vector<VertexRef> new_vertices;
	std::vector<FaceRef> new_faces;
	std::vector<EdgeRef> new_edges;
	std::vector<HalfedgeRef> new_halfedges;
	
	hf_tmp = f->halfedge;
	do {
		new_faces.push_back(emplace_face());

		new_vertices.push_back(emplace_vertex());
		new_vertices.push_back(hf_tmp->vertex); // Store the original one to interpolate

		new_edges.push_back(emplace_edge());
		new_edges.push_back(emplace_edge());
		new_edges.push_back(hf_tmp->edge); // Store the original one to interpolate

		new_halfedges.push_back(emplace_halfedge());
		new_halfedges.push_back(emplace_halfedge());
		new_halfedges.push_back(emplace_halfedge());
		new_halfedges.push_back(emplace_halfedge());
		new_halfedges.push_back(hf_tmp); // Store the original one to maintain the connectivity
		v_num++;
		hf_tmp = hf_tmp->next;
	} while (hf_tmp != f->halfedge);

	/**
	 * 2. Maintain the connectivity
	 */
	// New vertices 
	for (int i = 0; i < v_num; i++) {
		new_faces[i]->halfedge = new_halfedges[5 * i]; 
		// new_faces[i]->boundary = f->boundary; // TODO: Do we need reset this part?

		new_vertices[2 * i]->halfedge = new_halfedges[5 * i];
		new_vertices[2 * i]->position = new_vertices[2 * i + 1]->position;
		new_vertices[2 * i]->bone_weights = new_vertices[2 * i + 1]->bone_weights;
		
		new_edges[3 * i]->halfedge = new_halfedges[5 * i];
		new_edges[3 * i]->sharp = new_edges[3 * i  + 2]->sharp;
		new_edges[3 * i + 1]->halfedge = new_halfedges[5 * i + 1];
		new_edges[3 * i + 1]->sharp = new_edges[3 * i + 2]->sharp;

		// Process each halfedge's connectivity
		new_halfedges[5 * i]->next = new_halfedges[5 * i + 4];
		new_halfedges[5 * i]->twin = new_halfedges[5 * (((i - 1) % v_num + v_num) % v_num) + 2];
		new_halfedges[5 * i]->face = new_faces[i];
		new_halfedges[5 * i]->vertex = new_vertices[2 * i];
		new_halfedges[5 * i]->edge = new_edges[3 * i];

		new_halfedges[5 * i + 1]->next = new_halfedges[5 * i];
		new_halfedges[5 * i + 1]->twin = new_halfedges[5 * i + 3];
		new_halfedges[5 * i + 1]->face = new_faces[i];
		new_halfedges[5 * i + 1]->vertex = new_vertices[2 * ((i + 1) % v_num)];
		new_halfedges[5 * i + 1]->edge = new_edges[3 * i + 1];

		new_halfedges[5 * i + 2]->next = new_halfedges[5 * i + 1];
		new_halfedges[5 * i + 2]->twin = new_halfedges[5 * ((i + 1) % v_num)];
		new_halfedges[5 * i + 2]->face = new_faces[i];
		new_halfedges[5 * i + 2]->vertex = new_vertices[2 * ((i + 1) % v_num) + 1];
		new_halfedges[5 * i + 2]->edge = new_edges[3 * ((i + 1) % v_num)];

		new_halfedges[5 * i + 3]->next = new_halfedges[5 * ((i + 1) % v_num) + 3];
		new_halfedges[5 * i + 3]->twin = new_halfedges[5 * i + 1];
		new_halfedges[5 * i + 3]->face = f;
		new_halfedges[5 * i + 3]->vertex = new_vertices[2 * i];
		new_halfedges[5 * i + 3]->edge = new_edges[3 * i + 1];

		new_halfedges[5 * i + 4]->next = new_halfedges[5 * i + 2];
		new_halfedges[5 * i + 4]->face = new_faces[i];
		// TODO: we didn't interpolate the corner_uv and corner_normal here
	}
	
	f->boundary = false;
	f->halfedge = new_halfedges[3];
    return f;
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
// TODO there are some bugs
std::optional<Halfedge_Mesh::EdgeRef> Halfedge_Mesh::flip_edge(EdgeRef e) {	
	//A2L1: Flip Edge
	if (e->on_boundary()) {
		return std::nullopt;
	}

	{
		int degree = 0;
		HalfedgeRef v_h = e->halfedge->vertex->halfedge;
		do {
			degree++;
			v_h = v_h->twin->next;
		} while (v_h != e->halfedge->vertex->halfedge);
		if (degree == 2) {
			return std::nullopt;
		}
		degree = 0;
		v_h = e->halfedge->twin->vertex->halfedge;
		do {
			degree++;
			v_h = v_h->twin->next;
		} while (v_h != e->halfedge->twin->vertex->halfedge);
		if (degree == 2) {
			return std::nullopt;
		}
	}

	
	HalfedgeRef& h = e->halfedge;
	HalfedgeRef& t = h->twin;
	
	// update the vertex's halfedge
	h->vertex->halfedge = h->vertex->halfedge == h ? h->twin->next : h->vertex->halfedge;
	t->vertex->halfedge = t->vertex->halfedge == t ? t->twin->next : t->vertex->halfedge;
	
	// update the h and t
	HalfedgeRef h_next = h->next;
	HalfedgeRef t_next = t->next;
	HalfedgeRef h_next2 = h->next->next;
	HalfedgeRef t_next2 = t->next->next;
	HalfedgeRef h_last = h;
	HalfedgeRef t_last = t;
	while (h_last->next != h) h_last = h_last->next;
	while (t_last->next != t) t_last = t_last->next;

	// make sure face's halfedge is still valid after flip
	h->face->halfedge = h;
	t->face->halfedge = t;

	// update the new start vertex
	h->vertex = t_next2->vertex;
	t->vertex = h_next2->vertex;
	
	// update the face and nextabout the changed vertex
	h_next->face = t->face;
	t_next->face = h->face;
	h_next->next = t;
	t_next->next = h;
	h_last->next = t_next;
	t_last->next = h_next;

	// update the h and t
	h->next = h_next2;
	t->next = t_next2;
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
	/**
	 * TODO bugs: sometimes it works, sometines not
	 * The ugliest shit I have ever written.
	 * Just too tired to fix it...
	 */

	HalfedgeRef h = face->halfedge;
	HalfedgeRef tmp_hf = h;
	bool adj_to_boundary = false;
	do {
		if (tmp_hf->edge->on_boundary()) {
			adj_to_boundary = true;
			break;
		}
		tmp_hf = tmp_hf->next;
	} while (tmp_hf != h);

	if (!adj_to_boundary) {
		face->boundary = true;
		return face;
	}

	FaceRef boundary_face;
	std::set<std::pair<HalfedgeRef, HalfedgeRef>> last_next;
	std::vector<EdgeRef> edge_erase;
	std::vector<VertexRef> vertices_erase;

	tmp_hf = h;
	do {
		if (tmp_hf->edge->on_boundary()) {
			edge_erase.push_back(tmp_hf->edge);
			if (tmp_hf->twin->next->edge->on_boundary()
			 && tmp_hf->twin->next->twin->face == face) {
				vertices_erase.push_back(tmp_hf->vertex);
			}
			boundary_face = tmp_hf->twin->face;
			HalfedgeRef next_valid_in = tmp_hf;
			do {
				next_valid_in = next_valid_in->next;
			} while (next_valid_in->edge->on_boundary());

			HalfedgeRef last_valid_in = next_valid_in->twin;
			do {
				last_valid_in = last_valid_in->next->twin; 
			} while (!last_valid_in->edge->on_boundary());

			last_next.insert({last_valid_in, next_valid_in});

			HalfedgeRef next_valid_out = tmp_hf->twin;
			do {
				next_valid_out = next_valid_out->next;
			} while (next_valid_out->twin->face == face);

			HalfedgeRef last_valid_out = next_valid_out->twin;
			do {
				last_valid_out = last_valid_out->next->twin; 
			} while (last_valid_out->face != face);
			last_next.insert({last_valid_out, next_valid_out});
		}
		tmp_hf = tmp_hf->next;
	} while (tmp_hf != h);

	int boundary_num = (int) last_next.size() >> 1;
	std::vector<FaceRef> boundary_faces = {boundary_face, face};
	std::vector<HalfedgeRef> adjust_faces;
	for (int i = 0; i < boundary_num - 2; i++) {
		boundary_faces.push_back(emplace_face());
	}
	
	size_t idx = 0;
	for (const auto& ln : last_next) {
		ln.first->next = ln.second;
		if (idx++ < boundary_num) {
			adjust_faces.push_back(ln.first);
		}
	}
	idx = 0;
	for (auto ttt : adjust_faces) {
		tmp_hf = ttt;
		do {
			tmp_hf->face = boundary_faces[idx];
			tmp_hf = tmp_hf->next;
		} while (tmp_hf != ttt);
		boundary_faces[idx]->halfedge = ttt;
		boundary_faces[idx++]->boundary = true;
	}

	while (idx < boundary_faces.size()) {
		erase_face(boundary_faces[idx++]);
	}

	for (auto e : edge_erase) {
		HalfedgeRef he = e->halfedge;
		HalfedgeRef te = e->halfedge->twin;
		erase_edge(e);
		erase_halfedge(he);
		erase_halfedge(te);
	}

	for (auto v : vertices_erase) {
		erase_vertex(v);
	}

	face->boundary = true;

	return face;
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
	// TODO: invalid mesh
	HalfedgeRef tmp_hf;
	FaceRef final_f = v->halfedge->face;
	std::vector<HalfedgeRef> hf_wait_erase;
	std::vector<std::vector<HalfedgeRef>> hf_loop;
	std::vector<EdgeRef> edge_wait_erase;
	if (v->on_boundary()) {
		/**
		 * [READ]
		 * If the vertex is on boundary loop, reject the operation.
		 * But in the github page, it also worked, I don't get it :)
		 * But I still implement the github page one with some bugs :(
		 */
		// 
		return std::nullopt;
		/*
		tmp_hf = v->halfedge;
		HalfedgeRef ori_boundary_pre_hf;
		HalfedgeRef h = emplace_halfedge();
		HalfedgeRef t = emplace_halfedge();
		EdgeRef boundary_edge = emplace_edge();
		boundary_edge->halfedge = h;
		h->twin = t;
		t->twin = h;
		do {
			std::vector<HalfedgeRef> tmp_hf_vector;
			if (!tmp_hf->face->boundary) {
				HalfedgeRef tmp_tmp_hf = tmp_hf;
				if (tmp_hf->twin->face->boundary) {
					t->vertex = tmp_hf->twin->vertex;
				}
				
				do {
					tmp_hf_vector.push_back(tmp_tmp_hf);
					tmp_tmp_hf = tmp_tmp_hf->next;
					if (tmp_tmp_hf->twin->face->boundary) {
						h->vertex = tmp_tmp_hf->vertex;
						t->next = tmp_tmp_hf->twin->next;
					}
				} while (tmp_tmp_hf->next != tmp_hf);

				tmp_hf->twin->vertex->halfedge = tmp_hf->next;
				final_f = tmp_hf->face;
				final_f->halfedge = h;
			} else {
				tmp_hf_vector.push_back(h);
				t->face = tmp_hf->face;
				tmp_hf->face->halfedge = t;
				tmp_hf->twin->vertex->halfedge = h;
			}
			if (tmp_hf->twin->face->boundary) {
				ori_boundary_pre_hf = tmp_hf->twin;
			}
			hf_loop.push_back(tmp_hf_vector);
			hf_wait_erase.push_back(tmp_hf);
			hf_wait_erase.push_back(tmp_hf->twin);
			erase_edge(tmp_hf->edge);
			if (tmp_hf->face != final_f && !tmp_hf->face->boundary) {
				erase_face(tmp_hf->face);
			}
			tmp_hf = tmp_hf->twin->next;
		} while (tmp_hf != v->halfedge);
		for (int i = 0; i < hf_loop.size(); i++) {
			hf_loop[i].back()->next = hf_loop[(i + 1) % hf_loop.size()][0];
			for (auto hf : hf_loop[i]) {
				hf->face = final_f;
			}
			erase_halfedge(hf_wait_erase[2 * i]);
			erase_halfedge(hf_wait_erase[2 * i + 1]);
		}

		tmp_hf = t->next;
		while (tmp_hf->next != ori_boundary_pre_hf) {
			tmp_hf = tmp_hf->next;
		}
		tmp_hf->next = t;
		
		erase_vertex(v);
		return final_f;
		*/
	} else {
		tmp_hf = v->halfedge;
		do {
			HalfedgeRef tmp_tmp_hf = tmp_hf->next;
			std::vector<HalfedgeRef> tmp_hf_vector;
			do {
				tmp_hf_vector.push_back(tmp_tmp_hf);
				tmp_tmp_hf = tmp_tmp_hf->next;
			} while (tmp_tmp_hf->next != tmp_hf);
			hf_loop.push_back(tmp_hf_vector);

			hf_wait_erase.push_back(tmp_hf);
			hf_wait_erase.push_back(tmp_hf->twin);

			tmp_hf->twin->vertex->halfedge = tmp_hf->next;
			erase_edge(tmp_hf->edge);
			if (tmp_hf->face != final_f) {
				erase_face(tmp_hf->face);
			}
			tmp_hf = tmp_hf->twin->next;
		} while (tmp_hf != v->halfedge);

		// The test case is given in CW...
		Vec3 f_normal = v->halfedge->face->normal();
		Vec3 A = v->halfedge->vertex->position;
		Vec3 B = v->halfedge->next->vertex->position;
		Vec3 C = v->halfedge->next->next->vertex->position;
		Vec3 AB = B - A;
		Vec3 BC = C - B;
		Vec3 dot_product = cross(AB, BC) * f_normal;
		bool CCW = (dot_product.x + dot_product.y + dot_product.z > 0);

		final_f->halfedge = hf_loop[0][0];
		for (int i = 0; i < hf_loop.size(); i++) {
			if (!CCW) {
				hf_loop[i].back()->next = hf_loop[(i + 1) % hf_loop.size()][0];
			} else {
				hf_loop[i].back()->next = hf_loop[(i - 1 + hf_loop.size()) % hf_loop.size()][0];
			}
			for (auto hf : hf_loop[i]) {
				hf->face = final_f;
			}
			erase_halfedge(hf_wait_erase[2 * i]);
			erase_halfedge(hf_wait_erase[2 * i + 1]);
		}

		erase_vertex(v);
		return final_f;
	}

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
	HalfedgeRef h = e->halfedge;
	HalfedgeRef t = h->twin;

	if (e->on_boundary()) {
		HalfedgeRef tmp_hf = (h->face->boundary ? t : h)->next;
		do {
			if (tmp_hf->edge->on_boundary()) {
				return std::nullopt;
			}
			tmp_hf = tmp_hf->next;
		} while (tmp_hf == h || tmp_hf == t);
	} else {
		auto get_vertex_edge_number = [](VertexRef v) { 
			// only for vertex whose halfedge is not halfedges.end()
			int answer = 0;
			HalfedgeRef temp = v->halfedge;
			do {
				answer++;
				temp = temp->twin->next;
			} while (temp != v->halfedge);
			return answer;
		};

		if (get_vertex_edge_number(h->vertex) == 2 
		|| get_vertex_edge_number(t->vertex) == 2) {
			return std::nullopt;
		}

	}

	if (e->on_boundary()) {
		FaceRef erased_face = h->face;
		FaceRef merged_face = t->face;
		// assume h is inside
		if (e->halfedge->face->boundary) {
			t = e->halfedge;
			h = t->twin;
		}

		HalfedgeRef h_last = h;
		do {
			h_last = h_last->next;
			h_last->face = merged_face;
		} while (h_last->next != h);

		HalfedgeRef t_last = t;
		do {
			t_last = t_last->next;
		} while (t_last->next != t);

		h_last->next = t->next;
		t_last->next = h->next;

		h->vertex->halfedge = t->next;
		t->vertex->halfedge = h->next;
		merged_face->halfedge = h->next;

		erase_edge(e);
		erase_halfedge(h);
		erase_halfedge(t);
		erase_face(erased_face);
		return merged_face;
	} else {
		FaceRef merged_face = h->face;
		FaceRef erased_face = t->face;
		
		HalfedgeRef h_last = h;
		do {
			h_last = h_last->next;
		} while (h_last->next != h);
		
		HalfedgeRef t_last = t;
		do {
			t_last = t_last->next;
			t_last->face = merged_face;
		} while (t_last->next != t);

		h_last->next = t->next;
		t_last->next = h->next;

		h->vertex->halfedge = t->next;
		t->vertex->halfedge = h->next;
		merged_face->halfedge = h->next;

		erase_edge(e);
		erase_halfedge(h);
		erase_halfedge(t);
		erase_face(erased_face);

		return merged_face;
	}

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
	auto get_vertex_edge_number = [](VertexRef v) { 
		// only for vertex whose halfedge is not halfedges.end()
		int answer = 0;
		HalfedgeRef temp = v->halfedge;
		do {
			answer++;
			temp = temp->twin->next;
		} while (temp != v->halfedge);
		return answer;
	};
	
	if (get_vertex_edge_number(v_h) <= 2 || get_vertex_edge_number(v_t) <= 2) {
		return std::nullopt;
	}

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
		if (tmp_hf != h) {
			from_to_vh.push_back({tmp_hf, tmp_hf->twin});
		}	
		tmp_hf = tmp_hf->twin->next;
	} while (tmp_hf != v_h->halfedge);
	
	// t's start point
	tmp_hf = v_t->halfedge;
	do{
		if (tmp_hf != t) {
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
	h->face->halfedge = h->next;
	t->face->halfedge = t->next;
	wait_to_erase_halfedge.push_back(h);
	wait_to_erase_halfedge.push_back(t);

	/**
	 * Fifth: delete the triangle
	 */
	tmp_hf = v->halfedge;
	do {
		if (tmp_hf->next->next == tmp_hf) {
			// set the twin
			tmp_hf->twin->twin = tmp_hf->next->twin;
			tmp_hf->next->twin->twin = tmp_hf->twin;
			// Set the edge
			tmp_hf->next->twin->edge = tmp_hf->twin->edge;
			// Set the edge's new haledge
			tmp_hf->twin->edge->halfedge = tmp_hf->twin;
			// set the vertex's new halfedge
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

	// Some BUGS: we need to accept imperfect :)

	if (f->boundary) return std::nullopt;

	VertexRef new_vertex = emplace_vertex();
	std::vector<VertexCRef> const_old_vertices;
	std::vector<VertexRef> old_vertices;
	std::vector<VertexRef> vertices_adjust;
	std::vector<HalfedgeRef> halfedges_wait_erased;
	std::vector<HalfedgeRef> halfedges_last;
	std::vector<HalfedgeRef> halfedges_next;
	std::vector<EdgeRef> edges_wait_erased;
	HalfedgeRef hf_tmp = f->halfedge;

	do {
		if (hf_tmp->twin->face->boundary) {
			return std::nullopt;
		}
		int num = 0;
		HalfedgeRef hf_last = hf_tmp->twin;
		do {
			hf_last = hf_last->next;
			num += 1;
		} while (hf_last->next != hf_tmp->twin);

		// Cannot collapse the face when the relative face is a triangle
		// Otherwise the triangle will also be collapsed
		if (num == 2) {
			return std::nullopt;
		}
		hf_last->face->halfedge = hf_last;
		halfedges_last.push_back(hf_last);
		halfedges_next.push_back(hf_tmp->twin->next);

		old_vertices.push_back(hf_tmp->vertex);
		const_old_vertices.push_back(hf_tmp->vertex);
		halfedges_wait_erased.push_back(hf_tmp);
		halfedges_wait_erased.push_back(hf_tmp->twin);
		edges_wait_erased.push_back(hf_tmp->edge);
		hf_tmp = hf_tmp->next;
	} while (hf_tmp != f->halfedge);
	
	new_vertex->halfedge = hf_tmp->twin->next;
	interpolate_data(const_old_vertices, new_vertex);
	for (auto v : old_vertices) {
		new_vertex->position += v->position / (float) const_old_vertices.size();
		hf_tmp = v->halfedge;
		do {
			hf_tmp->vertex = new_vertex;
			hf_tmp = hf_tmp->twin->next;
		} while (hf_tmp != v->halfedge);
		erase_vertex(v);
	}
	assert(halfedges_last.size() == halfedges_next.size());
	for (int i = 0; i < halfedges_last.size(); i++) {
		halfedges_last[i]->next = halfedges_next[i];
		halfedges_last[i]->face->halfedge = halfedges_last[i];
	} 
	for (auto hf : halfedges_wait_erased) erase_halfedge(hf);
	for (auto edge : edges_wait_erased) erase_edge(edge);
	erase_face(f);

    return new_vertex;
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
	
	if (!e->on_boundary() || !e2->on_boundary()) return std::nullopt;

	HalfedgeRef e_h = e->halfedge->face->boundary ? e->halfedge : e->halfedge->twin;
	HalfedgeRef e2_h = e2->halfedge->face->boundary ? e2->halfedge : e2->halfedge->twin;

	HalfedgeRef tmp_hf = e_h;
	do {
		if (tmp_hf == e2_h) return std::nullopt;
		tmp_hf = tmp_hf->next;
	} while (tmp_hf != e_h);
	// if (tmp_hf == e2_h) return std::nullopt;
	// tmp_hf->next = e2_h->next;

	FaceRef f_final = e2_h->face;
	FaceRef erase_f = e_h->face;
	
	interpolate_data({e_h, e2_h->twin}, e_h);
	interpolate_data({e_h->twin, e2_h}, e_h->twin);
	interpolate_data({e_h->vertex, e2_h->twin->vertex}, e_h->vertex);
	interpolate_data({e_h->twin->vertex, e2_h->vertex}, e_h->twin->vertex);
	
	e_h->vertex->position = (e_h->vertex->position + e2_h->twin->vertex->position) / 2.0f;
	e_h->twin->vertex->position = (e_h->twin->vertex->position + e2_h->vertex->position) / 2.0f;
	erase_vertex(e2_h->vertex);
	erase_vertex(e2_h->twin->vertex);

	tmp_hf = e2_h;
	do {
		tmp_hf = tmp_hf->next;
	} while (tmp_hf->next != e2_h);
	HalfedgeRef e2_last = tmp_hf;

	tmp_hf = e2_h->twin;
	do {
		tmp_hf = tmp_hf->next;
	} while (tmp_hf->next != e2_h->twin);
	HalfedgeRef e2_twin_last = tmp_hf;

	tmp_hf = e_h;
	do {
		tmp_hf = tmp_hf->next;
	} while (tmp_hf->next != e_h);
	HalfedgeRef e_last = tmp_hf;

	// tmp_hf = e_h->twin;
	// do {
	// 	tmp_hf = tmp_hf->next;
	// } while (tmp_hf->next != e_h->twin);
	// HalfedgeRef e_twin_last = tmp_hf;
	
	e2_last->next = e_h->next;
	e2_twin_last->next = e_h;
	e_last->next = e2_h->next;
	e2_h->next->vertex = e_h->vertex;
	e2_h->twin->next->vertex = e_h->twin->vertex;
	e_h->face = e2_h->twin->face;
	e_h->face->halfedge = e_h->next;
	e_h->next = e2_h->twin->next;
	e2_h->twin->face->halfedge = e_h;

	erase_edge(e2_h->edge);
	erase_halfedge(e2_h->twin);
	erase_halfedge(e2_h);
	erase_face(erase_f);

	f_final->halfedge = e_last;
	tmp_hf = e_last;
	do {
		tmp_hf->face = f_final;
		tmp_hf = tmp_hf->next;
	} while (tmp_hf != e_last);

    return e;
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
	if (!std::isfinite(direction.x) || !std::isfinite(direction.y) || !std::isfinite(direction.z)) {
		return;
	}
	int idx = 0;
	HalfedgeRef tmp_hf = face->halfedge;
	do {
		Vec3 A = tmp_hf->twin->next->twin->vertex->position;
		Vec3 B = tmp_hf->vertex->position;
		Vec3 BA = B - A;
		Vec3 unit = (B - A) / (B - A).norm();
		float dot_product = dot(direction * distance, unit);
		float along_distance;
		if (dot_product == 0.0f) {
			along_distance = -distance;
		} else {
			along_distance = (direction * distance).norm_squared() / dot_product;
		}

		tmp_hf->vertex->position = start_positions[idx++] + unit * along_distance;
		tmp_hf = tmp_hf->next;
	} while (tmp_hf != face->halfedge);
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
	std::vector<VertexRef> all_vertices;
	std::vector<VertexCRef> const_vertices;
	HalfedgeRef h = face->halfedge;
	do {
		all_vertices.push_back(h->vertex);
		const_vertices.push_back(h->vertex);
		h = h->next;
	} while (h != face->halfedge);

	Vec3 center_position = Vec3(0.0f);
	for (auto v : all_vertices) {
		center_position += v->position / (float)all_vertices.size();
	}

	for (auto v : all_vertices) {
		v->position = center_position + (v->position - center_position) * (1 - shrink);
		interpolate_data(const_vertices, v);
		v->position += move;
	}
}

