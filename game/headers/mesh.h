#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <shader.h>

struct Vertex {
	const glm::vec3 position;
	const glm::vec3 normal;
	const glm::vec2 texCoords;
};

struct Polygon {
	const Vertex vertices[3];
	const glm::vec3 normal;
	const float d;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Polygon> polygons;
	std::vector<Texture> textures;

	// Constructor steals (moves) resources from the given vectors
	Mesh(
		std::vector<Vertex>& aVertices,
		std::vector<unsigned int>& aIndices,
		std::vector<Texture>& aTextures
	);
	void draw(const Shader& shader);

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	void setupMesh();
};

#endif
