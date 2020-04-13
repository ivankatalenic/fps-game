#include "game/headers/gui/framestats-display.hh"

#include <string>

/**
 * The position argument specifies the bottom left corner of the frame stats display.
 */
template<std::size_t STATS_LEN>
FrameStatsDisplay<STATS_LEN>::FrameStatsDisplay(
	FontRenderer& font_renderer, const FrameStats<STATS_LEN>& frame_stats, glm::vec2 pos, float font_size):
	font_renderer_{font_renderer}, frame_stats_{frame_stats}, pos_{pos}, font_size_{font_size} {

}

template<std::size_t STATS_LEN>
void FrameStatsDisplay<STATS_LEN>::draw() const {
	constexpr float FONT_SIZE{0.05f};
	constexpr glm::vec3 FONT_COLOR{1.0f, 1.0f, 0.2f};

	const double avg_fps{frame_stats_.getAVGFPS()};
	const std::string avg_fps_str{std::to_string(avg_fps)};
	font_renderer_.draw("Avg. FPS: " + avg_fps_str, FONT_SIZE, pos_ + glm::vec2{0.0f, FONT_SIZE}, FONT_COLOR);

	const double avg_frame_time_ms{frame_stats_.getAVGFrameTime() * 1000};
	const std::string avg_frame_time_str{std::to_string(avg_frame_time_ms)};
	font_renderer_.draw("Avg. Frame Time [ms]: " + avg_frame_time_str, FONT_SIZE, pos_, FONT_COLOR);
}
