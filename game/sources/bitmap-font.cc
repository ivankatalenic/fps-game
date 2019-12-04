#include <bitmap-font.h>

#include <filesystem>

BitmapFont::BitmapFont(std::filesystem::path bitmap_path) {
	// Load teh texture data into the OpenGL context
	int image_width, image_height, color_channels;
	unsigned char* data;
	unsigned int texture_id{0};

	// Load grayscale font
	constexpr int PER_PIXEL_COMP{1};
	data = stbi_load((directory + '/' + file).c_str(), &image_width, &image_height, &color_channels, PER_PIXEL_COMP);
	if (data != NULL) {
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
		std::cout << "Error: The font texture cannot be loaded! Path to the font: " << bitmap_path << std::endl;
		stbi_image_free(data);
	}
	return texture_id;
}

BitmapFont::~BitmapFont() {
	// Unload the loaded texture data from the OpenGL context
}
