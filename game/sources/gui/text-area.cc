#include "game/headers/gui/text-area.hh"

#include "game/headers/debug-help.hh"

#include <string_view>

TextArea::TextArea(
	const FontRenderer* renderer,
	glm::vec2 position,
	glm::vec2 dimension,
	float text_scale,
	glm::vec3 text_color
):
	_renderer{renderer},
	_position{position},
	_dimension{dimension},
	_text_scale{text_scale},
	_text_color{text_color}
{

}

void TextArea::addLine(const std::string& line) {
	_lines.push_back(line);
}

/**
 * Important: Do not use this method without checking the returned index!
 * The returned index can be -1 if there is no characters in a string!
 * 
 * If there are characters in a string, this method will always return
 * an index which is greater than or equal to 0, even if the
 * first character is too wide.
 */
int TextArea::getLastCharPosInRow(std::string_view str, float scale) const {
	if (str.length() < 1) {
		// There are not characters in a string_view.
		return -1;
	}
	/**
	 * Uses the binary search algorithm to find the character position.
	 */
	std::size_t left_bound{0}, right_bound{str.length() - 1};
	while (left_bound < right_bound) {
		std::size_t middle{(left_bound + right_bound + 1) / 2};
		if (_renderer->getStringLength(str.substr(0, middle + 1), _text_scale) <= _dimension.x) {
			left_bound = middle;
		} else {
			right_bound = middle - 1;
		}
	}
	return static_cast<int>(left_bound);
}

std::vector<std::string_view> TextArea::breakLine(std::string_view str, float scale) const {
	std::vector<std::string_view> lines;
	std::size_t start_pos{0};
	while (start_pos < str.length()) {
		const int ret{getLastCharPosInRow(str.substr(start_pos), scale)};
		if (ret == -1) {
			break;
		}
		const std::size_t end_pos{static_cast<std::size_t>(ret)};
		lines.push_back(str.substr(start_pos, end_pos + 1));
		start_pos = start_pos + end_pos + 1;
	}
	return lines;
}

void TextArea::draw() const {
	// Y-coordinate of the currently rendered row
	// Start from the bottom of the text box
	float curr_row_y{_position.y};
	// Start drawing from the last row to the first
	for (std::vector<std::string>::const_reverse_iterator it{_lines.crbegin()}; it != _lines.crend(); ++it) {
		// Check if the row could be fully displayed inside the area
		if (curr_row_y > _position.y + _dimension.y - _text_scale) {
			// No more rows can be displayed
			break;
		}
		const std::string_view line(*it);

		if (_renderer->getStringLength(line, _text_scale) > _dimension.x) {
			// The line spans more than one row
			std::vector<std::string_view> broken_lines{breakLine(line, _text_scale)};
			for (std::vector<std::string_view>::const_reverse_iterator bl_it{broken_lines.crbegin()};
					bl_it != broken_lines.crend(); ++bl_it) {
				_renderer->draw(*bl_it, _text_scale, {_position.x, curr_row_y}, _text_color);
				curr_row_y += _text_scale;
				// Check if the next row could be fully displayed inside the area
				if (curr_row_y > _position.y + _dimension.y - _text_scale) {
					break;
				}
			}
		} else {
			// The line spans one row
			_renderer->draw(line, _text_scale, {_position.x, curr_row_y}, _text_color);
			curr_row_y += _text_scale;
		}
	}
}
