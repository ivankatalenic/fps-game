#ifndef BITMAP_FONT_RENDERER_H
#define BITMAP_FONT_RENDERER_H

#include <font-renderer.h>

#include <glad/glad.h>

#include <bitmap-font.h>
#include <shader.h>

class BitmapFontRenderer : public FontRenderer {
public:
	BitmapFontRenderer(const BitmapFont& font, const Shader& bitmap_shader,
		float screen_width_height);

	void draw(std::string_view text, float scale, glm::vec2 position, glm::vec3 color) const override;
	float getStringLength(std::string_view text, float scale) const override;
private:
	const BitmapFont& _font;
	const Shader& _bitmap_shader;
	float _screen_width_height;
	GLuint _vao;
	GLuint _vbo;
};

#endif // BITMAP_FONT_RENDER_H
