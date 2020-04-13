#include "game/headers/gui/camerastats-display.hh"

CameraStatsDisplay::CameraStatsDisplay(
	FontRenderer& font_renderer, const Camera& camera, glm::vec2 pos, float font_size):
	font_renderer_{font_renderer}, camera_{camera}, pos_{pos}, font_size_{font_size} {

}

void CameraStatsDisplay::draw() const {
	constexpr glm::vec3 FONT_COLOR(1.0f, 1.0f, 1.0f);
	const std::string pos_str{formatPosition()};
	font_renderer_.draw(pos_str, font_size_, pos_, FONT_COLOR);
}

std::string CameraStatsDisplay::formatPosition() const {
	return "Position: (" + std::to_string(camera_.pos.x) + ", "
		+ std::to_string(camera_.pos.y) + ", "
		+ std::to_string(camera_.pos.z) + ")";
}
