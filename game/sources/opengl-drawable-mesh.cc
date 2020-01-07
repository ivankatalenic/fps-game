#include <opengl-drawable-mesh.h>

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

OpenGLDrawableMesh::OpenGLDrawableMesh(std::shared_ptr<Mesh> mesh, Shader& shader):
		mesh_{mesh}, shader_{shader} {
	setupVertices();
	setupTextures();
}

void OpenGLDrawableMesh::setupVertices() {
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);

	glBindVertexArray(vao_);

	// Copying vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, mesh_->vertices_.size() * sizeof(Vertex), &mesh_->vertices_[0], GL_STATIC_DRAW);

	// Pointer for a vertex's position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);

	// Pointer for a vertex's normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
	
	// Pointer for a vertex's texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, tex_coords));

	glBindVertexArray(0);
}

static unsigned int load_texture_from_file(const std::string& file_path);

void OpenGLDrawableMesh::setupTextures() {
	for (const Texture& texture : mesh_->textures_) {
		// Load the texture
		unsigned int texture_id{load_texture_from_file(texture.path)};
		// Save the texture
		opengl_textures_.push_back({texture_id, texture});
	}
}

void OpenGLDrawableMesh::draw() const {
	// Setting up the mesh's textures
	unsigned int diffuse_n{1u};
	unsigned int specular_n{1u};
	for (unsigned int i{0u}; i < opengl_textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		const std::string name{opengl_textures_[i].texture.type};

		if (name == "texture_diffuse") {
			number = std::to_string(diffuse_n++);
		} else if (name == "texture_specular") {
			number = std::to_string(specular_n++);
		}

		glUniform1i(glGetUniformLocation(shader_.id, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, opengl_textures_[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	// Setting the mesh's material properties
	Material material{mesh_->material_};
	shader_.setVec3("material.color_ambient", material.color_ambient);
	shader_.setVec3("material.color_diffuse", material.color_diffuse);
	shader_.setVec3("material.color_specular", material.color_specular);
	shader_.setFloat("material.shininess", material.shininess);

	// Drawing the mesh
	glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, mesh_->vertices_.size());
	glBindVertexArray(0);
}

static unsigned int load_texture_from_file(const std::string& file_path) {
	int image_width, image_height, color_channels;
	unsigned char* data;
	unsigned int texture_id{0};

	data = stbi_load(file_path.c_str(), &image_width, &image_height, &color_channels, 0);
	if (data != NULL) {
		GLenum format{GL_RGB};
		if (color_channels == 1) {
			format = GL_RED;
		} else if (color_channels == 3) {
			format = GL_RGB;
		} else if (color_channels == 4) {
			format = GL_RGBA;
		}

		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, image_width, image_height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	} else {
		std::cout << "Cannot load a texture data! Path: " << file_path << std::endl;
		stbi_image_free(data);
	}
	return texture_id;
}
