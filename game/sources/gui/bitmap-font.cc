#include "game/headers/gui/bitmap-font.hh"

#include "external/stb/stb_image.h"
#include "external/glad/glad.h"

#include "game/headers/debug-help.hh"

#include <iostream>

BitmapFont::BitmapFont(const std::string& bitmap_path, int rows, int columns,
		int first_ascii_sym, float width_height):
	_rows{rows}, _columns{columns},
	_first_ascii_sym{first_ascii_sym}, _width_height{width_height}
{
	// Load teh texture data into the OpenGL context
	int image_width, image_height, color_channels;
	unsigned char* data;

	// Load grayscale font
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	constexpr int PER_PIXEL_COMP{1};
	data = stbi_load(bitmap_path.c_str(), &image_width, &image_height, &color_channels, PER_PIXEL_COMP);
	if (data != NULL) {
		glGenTextures(1, &_texture_id);
		glBindTexture(GL_TEXTURE_2D, _texture_id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		std::cout << "Error: The font texture cannot be loaded! Path to the font: " << bitmap_path << std::endl;
		stbi_image_free(data);
	}
}

BitmapFont::~BitmapFont() {
	// Unload the loaded texture data from the OpenGL context
	glDeleteTextures(1, &_texture_id);
}

void BitmapFont::getCharPosition(char c, float* x, float* y) const {
	int seq_num{c - _first_ascii_sym};
	if (seq_num < 0 || seq_num > 95) {
		// Outside of the printable ASCII-characters subset
		seq_num = 95;
	}
	const int row{seq_num / _columns};
	const int column{seq_num % _columns};

	// Top left corner of the symbol
	*x = static_cast<float>(column) / _columns;
	*y = static_cast<float>(row) / _rows;
}

float BitmapFont::getWidthHeight() const {
	return _width_height;
}

int BitmapFont::getRows() const {
	return _rows;
}

int BitmapFont::getColumns() const {
	return _columns;
}

unsigned int BitmapFont::getTextureId() const {
	return _texture_id;
}
