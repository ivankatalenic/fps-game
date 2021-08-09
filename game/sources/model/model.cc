#include "game/headers/model/model.hh"

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/ext.hpp"

Model::Model(glm::vec3 pos, std::vector<std::shared_ptr<Mesh>>&& meshes):
	pos_{glm::translate(glm::mat4(1.0f), pos)},
	meshes_{std::move(meshes)} {
}

void Model::move(glm::vec3 trans) {
	pos_ = glm::translate(pos_, trans);
}
