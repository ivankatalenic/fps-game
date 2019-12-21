#include <model.h>

#include <cmath>
#include <cfloat>
#include <optional>
#include <memory>

#include <glm/glm.hpp>
#include <math-aux.h>

#include <debug-help.h>

struct collision_result {
	float relative_distance;
	glm::vec3 point;
	glm::vec3 normal;
};

Model::Model(const std::vector<std::shared_ptr<Mesh>>& ameshes, const std::vector<Light>& alights):
		meshes{ameshes}, lights{alights} {
}

/**
 * Returns a modified velocity vector if a collision
 * is detected.
 */
bool Model::checkCollision(
		glm::vec3 position,
		glm::vec3 step,
		glm::vec3& return_step,
		int recursion_depth
	) {

	return false;

	if (math_aux::is_zero(glm::length(step))) {
		return_step = glm::vec3(0.0f);
		return true;
	}

	constexpr int RECURSION_CUTOFF{5};
	if (recursion_depth >= RECURSION_CUTOFF) {
		debug::print_str("The recursion cutoff depth reached!");
		return_step = glm::vec3(0.0f);
		return true;
	}

	constexpr float bound_radius{1.0f};

	bool is_collision_detected{false};

	collision_result main_collision{};
	main_collision.relative_distance = FLT_MAX;

	for (std::shared_ptr<Mesh>& mesh : meshes) {
		for (Polygon& polygon : mesh->polygons) {
			
			bool is_triangle_collision{false};
			collision_result triangle_collision;

			triangle_collision.normal = polygon.normal;

			float plane_dist{glm::dot(triangle_collision.normal, position) + polygon.d};
			float polygon_d{polygon.d};
			// Adjust the triangle plane's normal to face the player
			// And adjust the plane distance
			// And adjust the polygon's d parameter
			if (plane_dist < 0.0f) {
				triangle_collision.normal = -triangle_collision.normal;
				plane_dist = -plane_dist;
				polygon_d = -polygon_d;
			}

			// Check if the sphere is inside the triangle's bounds
			constexpr float BOUND_RADIUS_TOLERANCE{1e-2};
			if (plane_dist < bound_radius - BOUND_RADIUS_TOLERANCE) {
				// Check if the projection is inside triangle
				const glm::vec3 projection(position - plane_dist * triangle_collision.normal);
				if (math_aux::is_point_inside_triangle(projection,
						polygon.vertices[0].position,
						polygon.vertices[1].position,
						polygon.vertices[2].position)) {
					// debug::print_str("Inside a triangle's plane");

					// Skip the triangle
					continue;
				}

				// Distance to the triangle's edges, and vertices
				bool skip_triangle{false};
				for (int i{0}; i < 3; i++) {
					glm::vec3 p_start(polygon.vertices[i].position);
					glm::vec3 p_end(polygon.vertices[(i + 1) % 3].position);
					if (math_aux::distance_to_line(position, p_start, p_end)
							< bound_radius - BOUND_RADIUS_TOLERANCE) {
						// Skip the triangle
						skip_triangle = true;
						break;
					}
				}
				if (skip_triangle) {
					debug::print_str("Inside a triangle's edge");
					continue;
				}
			}


			// Check if the sphere will hit triangle's parallel bound planes
			const float triangle_normal_dot_step{glm::dot(triangle_collision.normal, step)};
			// Check if the step vector faces the plane
			if (triangle_normal_dot_step < 0.0f) {
				// The step vector is not parallel with the plane
				// Relative distance to the first parallel plane
				triangle_collision.relative_distance = 
					(bound_radius - polygon_d - glm::dot(triangle_collision.normal, position))
						/ triangle_normal_dot_step;
				if (triangle_collision.relative_distance >= 0.0f
						&& triangle_collision.relative_distance <= 1.0f) {
					triangle_collision.point =
						position + triangle_collision.relative_distance * step;
					const glm::vec3 projection(
						triangle_collision.point - bound_radius * triangle_collision.normal
					);
					if (math_aux::is_point_inside_triangle(projection,
							polygon.vertices[0].position,
							polygon.vertices[1].position,
							polygon.vertices[2].position)) {
						is_triangle_collision = true;
						// debug::print_str("Hitting triangle's parallel plane");
					}
				}
			}


			// Check if the sphere will hit triangle's edges
			if (!is_triangle_collision) {
				bool is_edge_collision{false};
				collision_result edge_collision;
				edge_collision.relative_distance = FLT_MAX;

				// Check every edge of the triangle
				for (int i{0}; i < 3; i++) {
					const glm::vec3 p_start(polygon.vertices[i].position);
					const glm::vec3 p_end(polygon.vertices[(i + 1) % 3].position);
					if (auto status{
							math_aux::is_colliding_with_cylinder(position, step,
								p_start, p_end, bound_radius)
						}) {
						const float relative_distance{
							glm::length(*status - position) / glm::length(step)
						};
						const glm::vec3 axis(glm::normalize(p_end - p_start));
						const glm::vec3 normal(
							(*status - p_start)
								- glm::dot(*status - p_start, axis) * axis
						);
						const float normal_dot_step{glm::dot(normal, step)};
						if (relative_distance >= 0.0f && relative_distance <= 1.0f
								// && !math_aux::is_zero(normal_dot_step)
								&& normal_dot_step < 0.0f
								&& relative_distance < edge_collision.relative_distance) {
							is_edge_collision = true;
							edge_collision.relative_distance = relative_distance;
							edge_collision.point = *status;
							edge_collision.normal = normal;
							// debug::print_var(normal_dot_step, "normal_dot_step");
						}
					}
				} // for (edge)

				// Found an edge which collides with the step vector
				if (is_edge_collision) {
					triangle_collision = edge_collision;
					is_triangle_collision = true;

					// debug::print_str("Hitting a triangle's edge");
					// debug::print_var(triangle_collision.relative_distance, "triangle_collision.relative_distance");
					// debug::print_var(glm::dot(triangle_collision.normal, step), "triangle_normal_dot_step");
				}
			} // Triangle's edges collision check
			

			// Check if the sphere will hit triangle's vertices
			if (!is_triangle_collision) {
				bool is_vertex_collision{false};
				collision_result vertex_collision;
				vertex_collision.relative_distance = FLT_MAX;

				for (int i{0}; i < 3; i++) {
					const glm::vec3 vertex(polygon.vertices[i].position);
					if (auto status{
							math_aux::is_colliding_with_sphere(position, step,
								vertex, bound_radius)
						}) {
						const float relative_distance{
							glm::length(*status - position) / glm::length(step)
						};
						const glm::vec3 normal(*status - vertex);
						const float normal_dot_step{glm::dot(normal, step)};
						if (relative_distance >= 0.0f && relative_distance < 1.0f
								// && !math_aux::is_zero(normal_dot_step)
								&& normal_dot_step < 0.0f
								&& relative_distance < vertex_collision.relative_distance) {
							vertex_collision.relative_distance = relative_distance;
							vertex_collision.normal = normal;
							vertex_collision.point = *status;
							is_vertex_collision = true;
						}
					}
				} // for (vertex)

				// Found a vertex which collides with the step vector
				if (is_vertex_collision) {
					triangle_collision = vertex_collision;
					is_triangle_collision = true;
					// debug::print_str("Hitting triangle's vertex");
				}
			} // Triangle's vertices collision check

			if (is_triangle_collision
					&& triangle_collision.relative_distance
						< main_collision.relative_distance) {
				main_collision = triangle_collision;
				is_collision_detected = true;			
			}

		} // for (polygon)
	} // for (mesh)

	if (is_collision_detected) {
		debug::print_str("Collision detected!");

		// The vector from the position to the point of collision.
		// The vector before the collision point.
		const glm::vec3 step_before(main_collision.point - position);

		// The vector from the point of collision to the end of the step vector
		// The vector after the collision point.
		// Also know as the penetration vector.
		const glm::vec3 step_after(step - step_before);

		const float collision_normal_dot_step_after{glm::dot(main_collision.normal, step_after)};
		// The vector that is perpendicular to the surface of the point of collision.
		// The tangent vector to the collision surface.
		const glm::vec3 step_tangent(step_after
			- collision_normal_dot_step_after * main_collision.normal);

		constexpr float NORMAL_JUMP{1e-5f};
		// A vector used to split a player from the surface on which it stands.
		const glm::vec3 step_up(NORMAL_JUMP * main_collision.normal);

		const glm::vec3 step_resolution(step_tangent + step_up);

		debug::print_vec(step_before, "step_before");
		debug::print_vec(step_after, "step_after");
		debug::print_vec(step_tangent, "step_tangent");
		debug::print_vec(main_collision.normal, "normal");

		debug::print_var(main_collision.relative_distance, "relative_distance");
		debug::print_var(glm::dot(main_collision.normal, step), "normal_dot_step");
		debug::print_var(collision_normal_dot_step_after, "collision_normal_dot_step_after");

		// Don't do the new collision check if the resolution vector
		// can't collide with any other object.
		// The step_before vector leads to the first object
		// which collides with the step vector.
		// The step_tangent vector, which is calculated directly using
		// the step_after vector, could collide with some other
		// object, but only if its length is non zero.
		//
		// And
		//
		// Don't do the new collision check if the resolution vector
		// can't be accurately calculated because of the limited
		// floating point data type precision.
		// if (!math_aux::is_zero(glm::length(step_tangent))) {
			// Check if the resolution vector vector doesn't collide with other objects
			glm::vec3 recursion_return_step;
			bool is_new_collision_detected{
				checkCollision(
					position + step_before,
					step_resolution,
					recursion_return_step,
					++recursion_depth
				)
			};
			if (is_new_collision_detected) {
				return_step = step_before + recursion_return_step;
			} else {
				return_step = step_before + step_resolution;
			}
		// } else {
		// 	// The step_after vector, aka the penetration vector,
		// 	// is zero, hence use only a vector up to the point of collision,
		// 	// aka the step_before vector.

		// 	return_step = step_up + step_before;
		// }

		return true;
	}
	return false;
}
