#ifndef COLLISION_SOLVER_HH
#define COLLISION_SOLVER_HH

#include "game/headers/collision/collidable.hh"

#include <vector>

class CollisionSolver {
public:
	void addCollidable(Collidable* collidable);
	void checkForCollisions();
private:
	std::vector<Collidable*> collidables;

	void checkForCollision(Collidable* first, Collidable* second);
	
	void checkBoxBoxColliders(Collidable* box1, Collidable* box2);
	void checkBoxCapsuleColliders(Collidable* box, Collidable* capsule);
	void checkCapsuleCapsuleColliders(Collidable* capsule1, Collidable* capsule2);
};

#endif // COLLISION_SOLVER_HH
