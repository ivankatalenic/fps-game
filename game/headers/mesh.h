#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <shader.h>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
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
		std::vector<Texture> textures;

		Mesh(
			std::vector<Vertex>& aVertices,
			std::vector<unsigned int>& indices,
			std::vector<Texture>& textures
		);
		void draw(const Shader& shader);
	
	private:
		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;

		void setupMesh();
};

#endif
