#ifndef BITMAP_FONT_H
#define BITMAP_FONT_H

#include <string>

#include <glad/glad.h>

class BitmapFont {
public:
	BitmapFont(const std::string& bitmap_path, int rows, int columns,
		int first_ascii_sym, float width_height);
	~BitmapFont();

	void getCharPosition(char c, float* x, float* y) const;
	float getWidthHeight() const;
	int getRows() const;
	int getColumns() const;
	unsigned int getTextureId() const;
private:
	int _rows;
	int _columns;
	int _first_ascii_sym;
	float _width_height;
	GLuint _texture_id{0};
};

#endif // BITMAP_FONT_H
