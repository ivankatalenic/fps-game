#ifndef TEXT_AREA_H
#define TEXT_AREA_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <font-renderer.h>

class TextArea {
public:
	TextArea(const FontRenderer& renderer, glm::vec2 position, glm::vec2 dimension,
		float text_scale, glm::vec3 text_color);

	void addLine(const std::string& line);
	void draw();

	const FontRenderer& _renderer;
	glm::vec2 _position;
	glm::vec2 _dimension;
	float _text_scale;
	glm::vec3 _text_color;

	std::vector<std::string> _lines;
};

#endif // TEXT_AREA_H
