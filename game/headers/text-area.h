#ifndef TEXT_AREA_H
#define TEXT_AREA_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <font-renderer.h>

class TextArea {
public:
	/**
	 * The position argument specifies the bottom left corner of the text area.
	 */
	TextArea(const FontRenderer& renderer,
		glm::vec2 position, glm::vec2 dimension,
		float text_scale, glm::vec3 text_color);

	void addLine(const std::string& line);
	void draw() const;
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

#endif // TEXT_AREA_H
