#include <bitmap-font-renderer.h>

#include <string>

#include <glad/glad.h>

BitmapFontRenderer::BitmapFontRenderer(const BitmapFont& font, const Shader& bitmap_shader):
	_font(font), _bitmap_shader(bitmap_shader) {
	
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

BitmapFontRenderer::~BitmapFontRenderer() {

}

void BitmapFontRenderer::draw(const std::string& text, float scale,
		glm::vec2 position, glm::vec3 color) {
	_bitmap_shader.use();
	_bitmap_shader.setVec3("color", color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _font._texture_id);
	glBindVertexArray(_vao);

	float offset_x{0.0f};
	const float width{scale * _font._sym_width / _font._sym_height};
	const float height{scale};
	for (std::string::const_iterator c{text.begin()}; c != text.end(); ++c) {
		float tex_x, tex_y;
		_font.getCharPosition(*c, &tex_x, &tex_y);

		GLfloat vertices[6][4] = {
			{position.x + offset_x, 		position.y + height, 0.0f, 0.0f},
			{position.x + offset_x, 	    position.y, 		 0.0f, 1.0f},
			{position.x + offset_x + width, position.y, 		 1.0f, 1.0f},

			{position.x + offset_x, 		position.y + height, 0.0f, 0.0f},
			{position.x + offset_x + width, position.y, 		 1.0f, 1.0f},
			{position.x + offset_x + width, position.y + height, 1.0f, 0.0f}
		};

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		offset_x += width;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}
