#ifndef FRAMESTATS_DISPLAY_HH
#define FRAMESTATS_DISPLAY_HH

#include "external/glm/glm/glm.hpp"

#include "game/headers/gui/element.hh"
#include "game/headers/gui/font-renderer.hh"

template <std::size_t STATS_LEN>
class FrameStatsDisplay : public Element {
public:
	FrameStatsDisplay(FontRenderer& font_renderer, const FrameStats<STATS_LEN>& frame_stats, glm::vec2 pos, float font_size);

	void draw() const override;
private:
	FontRenderer& font_renderer_;
	const FrameStats<STATS_LEN>& frame_stats_;
	glm::vec2 pos_;
	float font_size_;
};

#include "game/sources/gui/framestats-display.inl"

#endif // FRAMESTATS_DISPLAY_HH
