#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum Direction {
	DIRECTION_UP 		= 0,
	DIRECTION_DOWN 		= 1,
	DIRECTION_LEFT 		= 2,
	DIRECTION_RIGHT 	= 3,
	DIRECTION_FORWARD 	= 4,
	DIRECTION_BACKWARD	= 5
};

class Camera {
public:
	// Variables
	float fov;
	float clipNear;
	float clipFar;

	float yaw;
	float pitch;

	glm::vec3 pos;
	glm::vec3 lookAt;
	glm::vec3 viewUp;

	float speed;
	glm::vec3 direction; // Normalized heading direction
	glm::vec3 directionVectors[6];
	
	float sensitivity;

	bool activeDirections[6];

	// Constructors
	Camera() = default;
	Camera(float aFov, float aClipNear, float aClipFar,
		float aYaw, float aPitch, glm::vec3 aPos,
		float aSpeed, float aSensitivity);

	// Methods
	void step(float timeDelta);
	void setDirection(Direction d, bool active);
	void swipe(float x, float y);
	void updateDirection();
};

#endif // CAMERA_H
