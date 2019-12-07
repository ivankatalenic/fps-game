#include <bitmap-font-renderer.h>

BitmapFontRenderer::BitmapFontRenderer(const BitmapFont& font, const Shader& bitmap_shader,
		float screen_width_height):
	_font{font}, _bitmap_shader{bitmap_shader}, _screen_width_height{screen_width_height} {
	
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_STREAM_DRAW);
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

		GLfloat vertices[6][4] = {
			{position.x + offset_x, 		 position.y + cell_h, tex_x, 		 tex_y		  },
			{position.x + offset_x, 	     position.y, 		  tex_x, 		 tex_y + tex_h},
			{position.x + offset_x + cell_w, position.y, 		  tex_x + tex_w, tex_y + tex_h},

			{position.x + offset_x, 		 position.y + cell_h, tex_x, 		 tex_y	 	  },
			{position.x + offset_x + cell_w, position.y, 		  tex_x + tex_w, tex_y + tex_h},
			{position.x + offset_x + cell_w, position.y + cell_h, tex_x + tex_w, tex_y	 	  }
		};

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		offset_x += (_font._width_height * scale / _screen_width_height);
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

}

float BitmapFontRenderer::getSymWidthHeight() const {
	return _font._width_height;
}
