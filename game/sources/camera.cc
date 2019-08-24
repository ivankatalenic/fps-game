#include <camera.h>

#include <cmath>

#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera() {

}

Camera::Camera(float aFov, float aClipNear, float aClipFar,
		float aYaw, float aPitch, glm::vec3 aPos,
		float aSpeed, float aSensitivity):

			fov{aFov},
			clipNear{aClipNear},
			clipFar{aClipFar},

			yaw{aYaw},
			pitch{aPitch},

			pos{aPos},
			lookAt{glm::vec3(cos(yaw) * sin(pitch), cos(pitch), sin(yaw) * sin(pitch))},
			viewUp{glm::vec3(-cos(yaw) * cos(pitch), sin(pitch), -sin(yaw) * cos(pitch))},

			speed{aSpeed},
			direction{glm::vec3(0.0f)},
			directionVectors{
				glm::vec3(0.0f, 1.0f, 0.0f), 	// Up
				glm::vec3(0.0f, -1.0f, 0.0f),	// Down
				glm::cross(viewUp, lookAt),		// Left
				glm::cross(lookAt, viewUp),		// Right
				lookAt,							// Front
				-lookAt							// Back
			},

			sensitivity{aSensitivity},

			activeDirections{false, false, false, false, false, false} {

}

void Camera::step(float timeDelta) {
	pos += timeDelta * speed * direction;
}

void Camera::setDirection(Direction d, bool active) {
	activeDirections[d] = active;
	updateDirection();
}

void Camera::swipe(float x, float y) {
	constexpr float pi{glm::pi<float>()};
	constexpr float twoPi{2.0f * pi};
	const float yawDelta{sensitivity * x};
	const float pitchDelta{sensitivity * y};
	yaw += yawDelta;
	if (yaw < 0.0f || yaw >= twoPi) {
		yaw = yaw - floor(yaw / twoPi) * twoPi;
	}
	if (pitch + pitchDelta < 0.0f) {
		pitch = 0.0f;
	} else if (pitch + pitchDelta > pi) {
		pitch = pi;
	} else {
		pitch += pitchDelta;
	}
	lookAt = glm::vec3(cos(yaw) * sin(pitch), cos(pitch), sin(yaw) * sin(pitch));
	viewUp = glm::vec3(-cos(yaw) * cos(pitch), sin(pitch), -sin(yaw) * cos(pitch));
	const glm::vec3 left{glm::cross(viewUp, lookAt)};
	
	directionVectors[2] = left;					// Left
	directionVectors[3] = -directionVectors[2];	// Right
	directionVectors[4] = lookAt;				// Front
	directionVectors[5] = -directionVectors[4];	// Back

	updateDirection();
}

void Camera::updateDirection() {
	// Update direction vector
	direction = glm::vec3(0.0f);
	for (unsigned int i{0u}; i < 6; i++) {
		if (activeDirections[i]) {
			direction += directionVectors[i];
		}
	}
	if (glm::length(direction) > 0.0f) {
		direction = glm::normalize(direction);
	}
}
