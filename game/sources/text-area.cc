#include <text-area.h>

#include <string_view>

TextArea::TextArea(const FontRenderer& renderer, glm::vec2 position, glm::vec2 dimension,
	float text_scale, glm::vec3 text_color):
		_renderer{renderer}, _position{position}, _dimension{dimension},
		_text_scale{text_scale}, _text_color{text_color} {

}

void TextArea::addLine(const std::string& line) {
	_lines.push_back(line);
}

void TextArea::draw() {
	float curr_row_y{_position.y};
	const int chars_in_row{_renderer.getCharsInRow(_dimension.x, _text_scale)};
	if (chars_in_row < 1) {
		return;
	}
	// Start drawing from the last row to the first
	for (std::vector<std::string>::const_reverse_iterator it{_lines.crbegin()}; it != _lines.crend(); ++it) {
		// Check if the row could be fully displayed inside the area
		if (curr_row_y > _position.y + _dimension.y - _text_scale) {
			break;
		}

		std::string_view line(*it);
		const int chars_in_string{static_cast<int>(line.length())};
		if (chars_in_string > chars_in_row) {
			// The line spans more than one row
			const int rows{chars_in_string / chars_in_row};
			for (int pos{rows * chars_in_row}; pos >= 0; pos -= chars_in_row) {
				if (pos + chars_in_row >= static_cast<int>(line.length())) {
					_renderer.draw(line.substr(pos), _text_scale, {_position.x, curr_row_y}, _text_color);
				} else {
					_renderer.draw(line.substr(pos, chars_in_row), _text_scale, {_position.x, curr_row_y}, _text_color);
				}
				curr_row_y += _text_scale;
				// Check if the next row could be fully displayed inside the area
				if (curr_row_y > _position.y + _dimension.y - _text_scale) {
					break;
				}
			}
		} else {
			// The line spans one row
			_renderer.draw(line, _text_scale, {_position.x, curr_row_y}, _text_color);
			curr_row_y += _text_scale;
		}
	}
}
