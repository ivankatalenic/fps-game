#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Polygon {
	Vertex vertices[3];
	glm::vec3 normal;
	float d;
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
	std::vector<Vertex> vertices;
	std::vector<Polygon> polygons;
	std::vector<Texture> textures;

	Material material;

	// Constructor steals (moves) resources from the given vectors
	Mesh(
		std::vector<Vertex>& aVertices,
		std::vector<Texture>& aTextures,
		Material amaterial
	);
};

#endif
