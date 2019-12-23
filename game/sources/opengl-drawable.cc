#include <opengl-drawable.h>

#include <glad/glad.h>
#include <stb_image.h>

OpenGLDrawable::OpenGLDrawable(std::shared_ptr<Mesh>& mesh, std::shared_ptr<Shader>& shader):
		_mesh{mesh}, _shader{shader} {
	setupVertices();
	setupTextures();
}

void OpenGLDrawable::setupVertices() {
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	glBindVertexArray(_vao);

	// Copying vertices
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _mesh->vertices.size() * sizeof(Vertex), &_mesh->vertices[0], GL_STATIC_DRAW);

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

static unsigned int load_texture_from_file(const std::string& file_path);

void OpenGLDrawable::setupTextures() {
	for (const Texture& texture : _mesh->textures) {
		// Load the texture
		unsigned int texture_id{load_texture_from_file(texture.path)};
		// Save the texture
		_opengl_textures.push_back({texture_id, texture});
	}
}

void OpenGLDrawable::draw() const {
	// Setting up the mesh's textures
	unsigned int diffuse_n{1u};
	unsigned int specular_n{1u};
	for (unsigned int i{0u}; i < _opengl_textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		const std::string name{_opengl_textures[i].texture.type};

		if (name == "texture_diffuse") {
			number = std::to_string(diffuse_n++);
		} else if (name == "texture_specular") {
			number = std::to_string(specular_n++);
		}

		glUniform1i(glGetUniformLocation(_shader->id, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, _opengl_textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	// Setting the mesh's material properties
	Material material{_mesh->material};
	_shader->setVec3("material.color_ambient", material.color_ambient);
	_shader->setVec3("material.color_diffuse", material.color_diffuse);
	_shader->setVec3("material.color_specular", material.color_specular);
	_shader->setFloat("material.shininess", material.shininess);

	// Drawing the mesh
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, _mesh->vertices.size());
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

std::vector<OpenGLDrawable> OpenGLDrawable::createDrawablesFromModel(
		std::shared_ptr<Model>& model, std::shared_ptr<Shader>& shader) {
	std::vector<OpenGLDrawable> drawables;
	for (std::shared_ptr<Mesh>& mesh : model->meshes) {
		drawables.emplace_back(mesh, shader);
	}
	return drawables;
}
