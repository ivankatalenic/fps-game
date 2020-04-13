#ifndef BITMAP_FONT_RENDERER_HH
#define BITMAP_FONT_RENDERER_HH

#include "game/headers/gui/font-renderer.hh"
#include "game/headers/gui/bitmap-font.hh"
#include "game/headers/renderer/opengl/shader.hh"

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
	unsigned int _vao;
	unsigned int _vbo;
};

#endif // BITMAP_FONT_RENDERER_HH
