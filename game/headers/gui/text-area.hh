#ifndef TEXT_AREA_HH
#define TEXT_AREA_HH

#include <vector>
#include <string>

#include "external/glm/glm/glm.hpp"
#include "game/headers/gui/font-renderer.hh"
#include "game/headers/gui/element.hh"

class TextArea : public Element {
public:
	/**
	 * The position argument specifies the bottom left corner of the text area.
	 */
	TextArea(const FontRenderer& renderer,
		glm::vec2 position, glm::vec2 dimension,
		float text_scale, glm::vec3 text_color);

	void addLine(const std::string& line);
	void draw() const override;
private:
	const FontRenderer& _renderer;
	glm::vec2 _position;
	glm::vec2 _dimension;
	float _text_scale;
	glm::vec3 _text_color;

	std::vector<std::string> _lines;

	int getLastCharPosInRow(std::string_view str, float scale) const;
	std::vector<std::string_view> breakLine(std::string_view str, float scale) const;
};

#endif // TEXT_AREA_HH
