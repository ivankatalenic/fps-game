#ifndef COLLIDABLE_HH
#define COLLIDABLE_HH

#include "external/glm/glm/glm.hpp"

#include <variant>

namespace Collider {

	struct Box {
		glm::vec3 vertices[8];
	};

	struct Capsule {
		float radius;
		glm::vec3 centers[2];
	};

}

class Collidable {
public:
	virtual std::variant<Collider::Box, Collider::Capsule> getCollider() = 0;
	virtual bool isMovable() = 0;
	virtual void move(float x, float y, float z) = 0;

	virtual ~Collidable() {};
};

#endif // COLLIDABLE_HH
