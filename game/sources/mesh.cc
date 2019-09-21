#include <mesh.h>

#include <utility>

#include <glad/glad.h>

Mesh::Mesh(
	std::vector<Vertex>& aVertices,
	std::vector<unsigned int>& aIndices,
	std::vector<Polygon>& aPolygons,
	std::vector<Texture>& aTextures
):	// Initialization
	vertices{std::move(aVertices)},
	indices{std::move(aIndices)},
	polygons{std::move(aPolygons)},
	textures{std::move(aTextures)} {
	// Constructor start
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
