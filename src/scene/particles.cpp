
#include "particles.h"

bool Particles::Particle::update(const PT::Aggregate &scene, Vec3 const &gravity, const float radius, const float dt) {

	//A4T4: particle update

	// Compute the trajectory of this particle for the next dt seconds.

	float t_remaining = dt;

	while (t_remaining > EPS_F) {
		// (1) Build a ray representing the particle's path as if it travelled at constant velocity.
		Ray ray = Ray(position, velocity.unit());

		// (2) Intersect the ray with the scene and account for collisions. Be careful when placing
		// collision points using the particle radius. Move the particle to its next position.
		PT::Trace trace = scene.hit(ray);

		if (!trace.hit) {
			t_remaining = std::min(t_remaining, age);
			position += velocity * t_remaining;
			velocity += gravity * t_remaining;
			age -= t_remaining;
			if (age < EPS_F) {
				return false; //particle is dead
			}
			break;
		}

		if (dot(trace.normal, velocity) > 0.f) {
			trace.normal = -trace.normal;
		}

		float cos_theta = dot(-velocity, trace.normal) / velocity.norm();
		const float eps = 1e-6f;
		if (cos_theta <= eps) {
			t_remaining = std::min(t_remaining, age);
			position += velocity * t_remaining;
			velocity += gravity * t_remaining;
			age -= t_remaining;
			if (age < EPS_F) {
				return false; //particle is dead
			}
			break;
		}
		float gap = radius / cos_theta;
		float hit_distance = trace.distance - gap;

		t_remaining = std::min(t_remaining, age);
		bool hit = t_remaining * velocity.norm() >= hit_distance;
		float used_time = hit ? (hit_distance / velocity.norm()) : t_remaining;
		if (hit_distance < 0.0f) {
			// If the hit distance is negative, it means the ray is already inside the object.
			// In this case, we can just move the particle to the collision point.
			used_time = 0.0f;
			hit = true; // we still hit the object, just inside it
		}

		if (hit && trace.hit) {
			position += velocity * used_time; //move to collision point
			velocity = 2 * dot(-velocity, trace.normal.unit()) * trace.normal.unit() + velocity; //reflect
		} else {
			position += velocity * used_time;
		}
		// (3) Account for acceleration due to gravity after updating position.
		Vec3 acceleration = gravity;
		velocity += acceleration * used_time;
		// (4) Repeat until the entire time step has been consumed.
		t_remaining -= used_time;
		// (5) Decrease the particle's age and return 'false' if it should be removed.
		age -= used_time;
		if (age < EPS_F) {
			return false; //particle is dead
		}
	}

	return true;
}

void Particles::advance(const PT::Aggregate& scene, const Mat4& to_world, float dt) {

	if(step_size < EPS_F) return;

	step_accum += dt;

	while(step_accum > step_size) {
		step(scene, to_world);
		step_accum -= step_size;
	}
}

void Particles::step(const PT::Aggregate& scene, const Mat4& to_world) {

	std::vector<Particle> next;
	next.reserve(particles.size());

	for(Particle& p : particles) {
		if(p.update(scene, gravity, radius, step_size)) {
			next.emplace_back(p);
		}
	}

	if(rate > 0.0f) {

		//helpful when emitting particles:
		float cos = std::cos(Radians(spread_angle) / 2.0f);

		//will emit particle i when i == time * rate
		//(i.e., will emit particle when time * rate hits an integer value.)
		//so need to figure out all integers in [current_step, current_step+1) * step_size * rate
		//compute the range:
		double begin_t = current_step * double(step_size) * double(rate);
		double end_t = (current_step + 1) * double(step_size) * double(rate);

		uint64_t begin_i = uint64_t(std::max(0.0, std::ceil(begin_t)));
		uint64_t end_i = uint64_t(std::max(0.0, std::ceil(end_t)));

		//iterate all integers in [begin, end):
		for (uint64_t i = begin_i; i < end_i; ++i) {
			//spawn particle 'i':

			float y = lerp(cos, 1.0f, rng.unit());
			float t = 2 * PI_F * rng.unit();
			float d = std::sqrt(1.0f - y * y);
			Vec3 dir = initial_velocity * Vec3(d * std::cos(t), y, d * std::sin(t));

			Particle p;
			p.position = to_world * Vec3(0.0f, 0.0f, 0.0f);
			p.velocity = to_world.rotate(dir);
			p.age = lifetime; //NOTE: could adjust lifetime based on index
			next.push_back(p);
		}
	}

	particles = std::move(next);
	current_step += 1;
}

void Particles::reset() {
	particles.clear();
	step_accum = 0.0f;
	current_step = 0;
	rng.seed(seed);
}

bool operator!=(const Particles& a, const Particles& b) {
	return a.gravity != b.gravity
	|| a.radius != b.radius
	|| a.initial_velocity != b.initial_velocity
	|| a.spread_angle != b.spread_angle
	|| a.lifetime != b.lifetime
	|| a.rate != b.rate
	|| a.step_size != b.step_size
	|| a.seed != b.seed;
}
