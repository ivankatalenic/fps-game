#include "game/headers/model/mesh.hh"

#include <utility>

Mesh::Mesh(std::vector<Triangle>&& triangles, std::vector<Texture>&& textures, Material material):
	triangles_{std::move(triangles)},
	textures_{std::move(textures)},
	material_{material} {
}
