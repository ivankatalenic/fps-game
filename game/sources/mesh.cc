#include <mesh.h>

#include <utility>

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<Texture>&& textures, Material material):
	vertices_{std::move(vertices)},
	textures_{std::move(textures)},
	material_{material} {
}
