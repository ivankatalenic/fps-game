#include "game/headers/model/mesh.hh"

#include "external/glm/glm/ext/matrix_transform.hpp"

#include <utility>

Mesh::Mesh(glm::vec3 pos, std::vector<Triangle>&& triangles, std::vector<Texture>&& textures, Material material):
	pos_{glm::translate(glm::mat4(1.0f), pos)},
	triangles_{std::move(triangles)},
	textures_{std::move(textures)},
	material_{material} {
}

void Mesh::move(glm::vec3 trans) {
	pos_ = glm::translate(pos_, trans);
}
