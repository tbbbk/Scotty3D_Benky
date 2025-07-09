
#include "../geometry/spline.h"

template<typename T> T Spline<T>::at(float time) const {

	// A4T1b: Evaluate a Catumull-Rom spline
	if (has(time)) {
		return knots.at(time); // If time is a knot, return its value
	}

	if (!any()) {
		return T(); // No knots, return default value
	}

	if (knots.size() == 1) {
		return knots.begin()->second; // Only one knot, return its value
	}

	if (knots.begin()->first >= time) {
		return knots.begin()->second; // Time is before the first knot
	}

	if (knots.rbegin()->first <= time) {
		return knots.rbegin()->second; // Time is after the last knot
	}
	// Given a time, find the nearest positions & tangent values
	// defined by the control point map.
	T p0, p1, p2, p3, m1, m2;
	float k0, k1, k2, k3;
	
	k2 = knots.upper_bound(time)->first; // Find the first knot after 'time'
	p2 = knots.at(k2); // Position at k1
	k1 = std::prev(knots.upper_bound(time))->first; // Find the knot before 'time'
	p1 = knots.at(k1); // Position at k0


	if (k1 == knots.begin()->first) {
		k0 = k1 - (k2 - k1);
		p0 = p1 - (p2 - p1);
	} else {
		k0 = std::prev(knots.find(k1))->first;
		p0 = std::prev(knots.find(k1))->second; // Position at k0
	}

	if (k2 == knots.rbegin()->first) {
		k3 = k2 + (k2 - k1);
		p3 = p2 + (p2 - p1);
	} else {
		k3 = std::next(knots.find(k2))->first;
		p3 = std::next(knots.find(k2))->second; // Position at k2
	}

	m1 = (p2 - p0) / (k2 - k0); // Tangent at k0
	m2 = (p3 - p1) / (k3 - k1); // Tangent at k1
	// Transform them for use with cubic_unit_spline
	// interpolate the time
	float t = (time - k1) / (k2 - k1); // Normalized time in [0,1]
	T p_interpolate = cubic_unit_spline(t, p1, p2, m1, m2);
	// Be wary of edge cases! What if time is before the first knot,
	// before the second knot, etc...

	return p_interpolate; // Return the interpolated value
}

template<typename T>
T Spline<T>::cubic_unit_spline(float time, const T& position0, const T& position1,
                               const T& tangent0, const T& tangent1) {

	// A4T1a: Hermite Curve over the unit interval

	// Given time in [0,1] compute the cubic spline coefficients and use them to compute
	// the interpolated value at time 'time' based on the positions & tangents

	// Note that Spline is parameterized on type T, which allows us to create splines over
	// any type that supports the * and + operators.
	float t_cubic = time * time * time; // t^3
	float t_squared = time * time; // t^2
	float h_00 = 2 * t_cubic - 3 * t_squared + 1; // h00(t)
	float h_10 = t_cubic - 2 * t_squared + time; // h10(t)
	float h_01 = -2 * t_cubic + 3 * t_squared; // h01(t)
	float h_11 = t_cubic - t_squared; // h11(t)

	return T(h_00 * position0 + h_10 * tangent0 + h_01 * position1 + h_11 * tangent1);
}

template class Spline<float>;
template class Spline<double>;
template class Spline<Vec4>;
template class Spline<Vec3>;
template class Spline<Vec2>;
template class Spline<Mat4>;
template class Spline<Spectrum>;
