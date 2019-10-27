#include <mesh.h>

#include <utility>

#include <glad/glad.h>

#include <iostream>
#include <iomanip>

Mesh::Mesh(
	std::vector<Vertex>& aVertices,
	std::vector<unsigned int>& aIndices,
	std::vector<Texture>& aTextures
):	// Initialization
	vertices{std::move(aVertices)},
	indices{std::move(aIndices)},
	textures{std::move(aTextures)} {
	// Constructor start

	for (unsigned long long i{0}; i < indices.size(); i += 3ull) {
		const Vertex A{vertices[indices[i + 0ull]]};
		const Vertex B{vertices[indices[i + 1ull]]};
		const Vertex C{vertices[indices[i + 2ull]]};
		const glm::vec3 cross_product(glm::cross(B.position - A.position, C.position - A.position));
		glm::vec3 normal(1.0f, 1.0f, 1.0f);
		if (glm::length(cross_product) < 0.0001f) {
			std::cout << "A polygon's normal has length equal to 0!" << std::endl;
		} else {
			normal = glm::normalize(cross_product);
		}
		const float d{-glm::dot(normal, A.position)};
		polygons.push_back({{A, B, C}, normal, d});
	}
	
	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Pointer for vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	// Pointer for vertex normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
	// Pointer for vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::draw(const Shader& shader) {

	unsigned int diffuseNr{1u};
	unsigned int specularNr{1u};

	for (unsigned int i{0u}; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string& name{textures[i].type};

		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		} else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}

		glUniform1i(glGetUniformLocation(shader.id, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	// Drawing mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
