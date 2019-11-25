#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <shader.h>

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
	unsigned int id;
	std::string type;
	std::string path;
};

struct Light;

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
	void draw(std::shared_ptr<Shader> shader);

private:
	unsigned int VAO;
	unsigned int VBO;

	void setupMesh();
};

#endif
