#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include <string_view>

#include <glm/glm.hpp>

class FontRenderer {
public:
	/**
	 * The position argument specifies the bottom left corner of the text component.
	 */
	virtual void draw(std::string_view text, float scale, glm::vec2 position, glm::vec3 color) const = 0;
	virtual float getStringLength(std::string_view text, float scale) const = 0;
	// Enabling a derived class to properly destruct itself
	virtual ~FontRenderer() {};
};

#endif // FONT_RENDERER_H
