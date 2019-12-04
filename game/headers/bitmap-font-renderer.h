#ifndef BITMAP_FONT_RENDERER_H
#define BITMAP_FONT_RENDERER_H

#include <bitmap-font.h>

class BitmapFontRenderer {
public:
	BitmapFontRenderer(const BitmapFont& font);
	~BitmapFontRenderer();
	void draw(const std::string& text, glm::vec2 position, glm::vec2 resolution, glm::vec3 color);
private:
};

#endif // BITMAP_FONT_RENDER_H
