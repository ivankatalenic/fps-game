#include <mesh.h>
#include <model.h>

#include <utility>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <math-aux.h>

#include <iostream>
#include <iomanip>

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
	
	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	// Copying vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Pointer for a vertex's position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);

	// Pointer for a vertex's normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
	
	// Pointer for a vertex's texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::draw(std::shared_ptr<Shader> shader) {

	// Setting textures
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

		glUniform1i(glGetUniformLocation(shader->id, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	// Setting a material properties
	shader->setVec3("material.color_ambient", material.color_ambient);
	shader->setVec3("material.color_diffuse", material.color_diffuse);
	shader->setVec3("material.color_specular", material.color_specular);
	shader->setFloat("material.shininess", material.shininess);

	// Drawing a mesh
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}
