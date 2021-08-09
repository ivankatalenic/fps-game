#ifndef MODEL_HH
#define MODEL_HH

#include "game/headers/model/mesh.hh"

#include <vector>
#include <memory>

class Model {
public:
	glm::mat4 pos_;
	
	std::vector<std::shared_ptr<Mesh>> meshes_;

	Model(glm::vec3 pos, std::vector<std::shared_ptr<Mesh>>&& meshes);

	void move(glm::vec3 trans);
};

#endif // MODEL_HH
