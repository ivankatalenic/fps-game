#include "game/headers/collision/collision-solver.hh"

#include "game/headers/collision/collidable.hh"

#include <vector>
#include <variant>

void CollisionSolver::addCollidable(Collidable* collidable) {
	collidables.push_back(collidable);
}

void CollisionSolver::checkForCollisions() {
	const std::size_t collidables_count{collidables.size()};
	for (std::size_t i{0}; i < collidables_count - 1; ++i) {
		for (std::size_t j{i + 1}; j < collidables_count; ++j) {
			checkForCollision(collidables[i], collidables[j]);
		}
	}
}

void CollisionSolver::checkForCollision(Collidable* first, Collidable* second) {
	std::variant<Collider::Box, Collider::Capsule> firstCollider{first->getCollider()};
	std::variant<Collider::Box, Collider::Capsule> secondCollider{second->getCollider()};

	if (std::holds_alternative<Collider::Box>(firstCollider)) {
		if (std::holds_alternative<Collider::Box>(secondCollider)) {
			checkBoxBoxColliders(first, second);
		} else {
			checkBoxCapsuleColliders(first, second);
		}
	} else {
		if (std::holds_alternative<Collider::Box>(secondCollider)) {
			checkBoxCapsuleColliders(second, first);
		} else {
			checkCapsuleCapsuleColliders(first, second);
		}
	}
}

void CollisionSolver::checkBoxBoxColliders(Collidable* box1, Collidable* box2) {
	// TODO
}

void CollisionSolver::checkBoxCapsuleColliders(Collidable* box, Collidable* capsule) {
	// TODO
}

void CollisionSolver::checkCapsuleCapsuleColliders(Collidable* capsule1, Collidable* capsule2) {
	// TODO
}

