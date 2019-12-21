#include <mesh.h>
#include <model.h>

#include <utility>

#include <glm/glm.hpp>
#include <math-aux.h>

#include <iostream>

Mesh::Mesh(
	std::vector<Vertex>& aVertices,
	std::vector<Texture>& aTextures,
	Material amaterial
):	// Initialization
		vertices{std::move(aVertices)},
		textures{std::move(aTextures)},
		material{amaterial} {
	// Constructor start

	for (unsigned long long i{0}; i < vertices.size(); i += 3ull) {
		const Vertex A{vertices[i + 0ull]};
		const Vertex B{vertices[i + 1ull]};
		const Vertex C{vertices[i + 2ull]};
		const glm::vec3 cross_product(glm::cross(B.position - A.position, C.position - A.position));
		glm::vec3 normal(1.0f, 1.0f, 1.0f);
		if (math_aux::is_zero(glm::length(cross_product))) {
			std::cout << "A polygon's normal has length equal to 0!" << std::endl;
		} else {
			normal = glm::normalize(cross_product);
		}
		const float d{-glm::dot(normal, A.position)};
		polygons.push_back({{A, B, C}, normal, d});
	}
}
