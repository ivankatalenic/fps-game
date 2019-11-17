#ifndef MATH_AUX_H
#define MATH_AUX_H

#include <cmath>
#include <optional>

#include <glm/glm.hpp>

namespace math_aux {

	inline bool is_zero(float value) {
		constexpr float ZERO_TOLERANCE{1e-5f};
		if (fabs(value) < ZERO_TOLERANCE) {
			return true;
		}
		return false;
	}

	inline bool is_point_inside_triangle(glm::vec3 T,
			glm::vec3 A, glm::vec3 B, glm::vec3 C) {
		const float two_surface{
			glm::length(glm::cross(B - A, C - A))
		};

		const float t1{glm::length(glm::cross(A - T, B - T)) / two_surface};
		const float t2{glm::length(glm::cross(B - T, C - T)) / two_surface};
		const float t3{glm::length(glm::cross(C - T, A - T)) / two_surface};
		constexpr float error_tolerance{1e-5f};
		if (fabs(t1 + t2 + t3 - 1.0f) > error_tolerance) {
			return false;
		}
		return true;
	}

	inline std::optional<glm::vec3> is_colliding_with_sphere(glm::vec3 pos, glm::vec3 direction,
			glm::vec3 center, float r) {
		const float a{glm::dot(direction, direction)};
		const glm::vec3 center_pos(pos - center);
		const float b{2.0f * glm::dot(center_pos, direction)};
		const float c{glm::dot(center_pos, center_pos) - r * r};
		const float d{b * b - 4.0f * a * c};
		if (d <= 0.0f) {
			return {};
		}
		const float lambda{(-b - static_cast<float>(sqrt(d))) / (2.0f * a)};
		return pos + lambda * direction;
	}

	inline std::optional<glm::vec3> is_colliding_with_cylinder(glm::vec3 pos, glm::vec3 direction,
			glm::vec3 p_start, glm::vec3 p_end, float r) {
		const glm::vec3 axis(p_end - p_start);
		const glm::vec3 p0(pos - p_start);
		const float direction_dot_axis{glm::dot(direction, axis)};
		const float axis_dot_axis{glm::dot(axis, axis)};
		const float a{
			glm::dot(direction, direction) 
				- (direction_dot_axis * direction_dot_axis) / axis_dot_axis
		};
		const float p0_dot_axis{glm::dot(p0, axis)};
		const float b{
			glm::dot(p0, direction) - (p0_dot_axis * direction_dot_axis) / axis_dot_axis
		};
		const float c{
			glm::dot(p0, p0) - r * r - (p0_dot_axis * p0_dot_axis) / axis_dot_axis
		};
		const float d{b * b - a * c};
		if (d <= 0.0f) {
			return {};
		}
		const float lambda{(-b - static_cast<float>(sqrt(d))) / a};
		const glm::vec3 intersection(pos + lambda * direction);
		const float projection{glm::dot(intersection - p_start, axis)};
		if (projection > 0.0f && projection < axis_dot_axis) {
			return intersection;
		}
		return {};
	}

	inline float distance_to_sphere(glm::vec3 pos, glm::vec3 center, float r) {
		const float distance{glm::length(center - pos) - r};
		return distance >= 0.0f ? distance : 0.0f;
	}

	inline float distance_to_line(glm::vec3 pos, glm::vec3 p_start, glm::vec3 p_end) {
		const glm::vec3 direction(p_end - p_start);
		const float lambda{glm::dot(pos - p_start, direction) / glm::dot(direction, direction)};
		if (lambda < 0.0f) {
			return glm::length(pos - p_start);
		} else if (lambda > 1.0f) {
			return glm::length(pos - p_end);
		}
		const glm::vec3 t(p_start + lambda * direction);
		return glm::length(pos - t);
	}

} // namespace math_aux

#endif // MATH_AUX_H
