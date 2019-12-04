#ifndef BITMAP_FONT_RENDERER_H
#define BITMAP_FONT_RENDERER_H

#include <string>

#include <glm/glm.hpp>

#include <bitmap-font.h>
#include <shader.h>

class BitmapFontRenderer {
public:
	BitmapFontRenderer(const BitmapFont& font, const Shader& bitmap_shader);
	~BitmapFontRenderer();
	void draw(const std::string& text, float scale, glm::vec2 position, glm::vec3 color);
private:
	const BitmapFont& _font;
	const Shader& _bitmap_shader;
	unsigned int _vao;
	unsigned int _vbo;
};

#endif // BITMAP_FONT_RENDER_H
