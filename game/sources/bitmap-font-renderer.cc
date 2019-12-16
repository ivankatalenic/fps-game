#include <bitmap-font-renderer.h>

BitmapFontRenderer::BitmapFontRenderer(const BitmapFont& font, const Shader& bitmap_shader,
		float screen_width_height):
	_font{font}, _bitmap_shader{bitmap_shader}, _screen_width_height{screen_width_height} {
	
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	GLfloat vertices[6][4]{
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
		{1.0f, 0.0f, 1.0f, 1.0f},

		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 0.0f}
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BitmapFontRenderer::draw(std::string_view text, float scale,
		glm::vec2 position, glm::vec3 color) const {

	_bitmap_shader.use();
	_bitmap_shader.setVec3("color", color);
	_bitmap_shader.setInt("tex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _font._texture_id);
	glBindVertexArray(_vao);

	glDisable(GL_DEPTH_TEST);
	// Enable the blending for text rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float offset_x{0.0f};
	const float tex_w{1.0f / _font._columns};
	const float tex_h{1.0f / _font._rows};
	const float cell_h{scale};
	const float cell_w{cell_h * static_cast<float>(_font._rows) / (_font._columns * _screen_width_height)};
	for (std::string_view::const_iterator c{text.cbegin()}; c != text.end(); ++c) {
		float tex_x, tex_y;
		_font.getCharPosition(*c, &tex_x, &tex_y);
		// Update the position matrix uniform
		glm::mat3 pos_matrix(
			cell_w, 0.0f, 0.0f,
			0.0f, cell_h, 0.0f,
			position.x + offset_x, position.y, 1.0f
		);
		_bitmap_shader.setMat3("pos_matrix", pos_matrix);
		// Update the texture matrix uniform
		glm::mat3 tex_matrix(
			tex_w, 0.0f, 0.0f,
			0.0f, tex_h, 0.0f,
			tex_x, tex_y, 1.0f
		);
		_bitmap_shader.setMat3("tex_matrix", tex_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		offset_x += (_font._width_height * scale / _screen_width_height);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

}

int BitmapFontRenderer::getCharsInRow(float row_length, float text_height) const {
	return static_cast<int>((row_length * _screen_width_height) / (_font._width_height * text_height));
}
