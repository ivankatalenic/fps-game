#ifndef MESH_HH
#define MESH_HH

#include "external/glm/glm/glm.hpp"

#include <vector>
#include <string>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

struct Triangle {
	Vertex vertices[3];
};

struct Material {
	glm::vec3 color_ambient;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
	float shininess;
};

struct Texture {
	std::string type;
	std::string path;
};

class Mesh {
public:
	glm::mat4 trans_matrix_{glm::mat4(1.0f)};

	std::vector<Triangle> triangles_;
	std::vector<Texture> textures_;

	Material material_;

	/**
	 * Mesh constructor steals (moves) resources from the given vectors.
	 */
	Mesh(
		std::vector<Triangle>&& triangles,
		std::vector<Texture>&& textures,
		Material material
	);
};

#endif // MESH_HH
