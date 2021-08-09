#ifndef CAMERASTATS_DISPLAY_HH
#define CAMERASTATS_DISPLAY_HH

#include "external/glm/glm/glm.hpp"

#include "game/headers/gui/element.hh"
#include "game/headers/gui/font-renderer.hh"
#include "game/headers/renderer/camera.hh"

#include <string>

class CameraStatsDisplay : public Element {
public:
	CameraStatsDisplay(
		FontRenderer* font_renderer,
		const Camera* camera,
		glm::vec2 pos,
		float font_size
	);

	void draw() const override;
private:
	FontRenderer* font_renderer_;
	const Camera* camera_;
	glm::vec2 pos_;
	float font_size_;

	std::string formatPosition() const;
};

#include "game/sources/gui/camerastats-display.inl"

#endif // CAMERASTATS_DISPLAY_HH
