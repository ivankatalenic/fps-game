#include "fps-game-config.h"

// Library classes
#include "external/glm/glm/gtc/constants.hpp"

// Local classes
#include "game/headers/service-locator.hh"
#include "game/headers/debug-help.hh"
#include "game/headers/frame-stats.hh"
#include "game/headers/window-manager.hh"

#include "game/headers/utility/logger.hh"

#include "game/headers/renderer/screen.hh"
#include "game/headers/renderer/camera.hh"
#include "game/headers/renderer/terrain-renderer.hh"

#include "game/headers/model/model.hh"
#include "game/headers/model/terrain-loader.hh"

#include "game/headers/gui/bitmap-font.hh"
#include "game/headers/gui/bitmap-font-renderer.hh"
#include "game/headers/gui/text-area.hh"
#include "game/headers/gui/scene.hh"
#include "game/headers/gui/framestats-display.hh"
#include "game/headers/gui/camerastats-display.hh"

#include "game/headers/input/keyboard-handler.hh"
#include "game/headers/input/mouse-handler.hh"
#include "game/headers/input/key.hh"

// System classes
#include <string>
#include <memory>
#include <tuple>

// Function prototypes
void reg_kb_listeners(
	std::shared_ptr<Logger>          logger,
	std::shared_ptr<KeyboardHandler> keyboard_handler,
	Camera&                          camera,
	TextArea&                        text_area,
	std::shared_ptr<WindowManager>   window_manager
);
void reg_mouse_listeners(
	std::shared_ptr<Logger>       logger,
	std::shared_ptr<MouseHandler> mouse_handler,
	Camera&                       camera,
	TextArea&                     text_area,
	Screen                        screen
);
std::tuple<Scene*, TextArea*> create_gui(
	Camera*          camera,
	FrameStats<512>* frame_stats,
	Screen           screen
);

int main() {
	const std::string title{"The Game"};
	std::shared_ptr<Logger> logger{ServiceLocator::getInstance().getLogger()};
	std::shared_ptr<KeyboardHandler> keyboard_handler{std::make_shared<KeyboardHandler>()};
	std::shared_ptr<MouseHandler> mouse_handler{std::make_shared<MouseHandler>()};
	std::shared_ptr<WindowManager> window_manager{
		std::make_shared<WindowManager>(logger, keyboard_handler, mouse_handler, title)
	};

	// Setting up a 3D camera
	Camera camera(
		glm::pi<float>() / 2.0f, 	 // FOV in radians
		0.1f,						 // Near clip plane
		500.0f,						 // Far clip plane
		0.0f,						 // Orientation Yaw: Angle from x-axis
		0.5f * glm::pi<float>(),	 // Orientation Pitch: Elevation from x-z plane
		glm::vec3(2.0f, 2.0f, 2.0f), // Position
		4.0f,						 // Movement speed
		0.001f						 // Mouse sensitivity
	);


	// Setting the GUI
	FrameStats<512> frame_stats;
	std::tuple<Scene*, TextArea*> gui_tuple{create_gui(&camera, &frame_stats, window_manager->get_screen())};
	Scene* GUI{std::get<0>(gui_tuple)};
	TextArea* text_area{std::get<1>(gui_tuple)};

	// Setting up a terrain loader, and a terrain renderer
	std::shared_ptr<TerrainRenderer> terrain_renderer{ServiceLocator::getInstance().getTerrainRenderer()};
	terrain_renderer->init(window_manager->get_screen(), &camera);
	std::shared_ptr<TerrainLoader> terrain_loader{ServiceLocator::getInstance().getTerrainLoader()};
	std::shared_ptr<Terrain> terrain{terrain_loader->loadTerrain("game/terrains/plane-cube/plane-cube.obj")};
	terrain_renderer->addTerrain(terrain);

	// Setting up inputs
	reg_kb_listeners(logger, keyboard_handler, camera, *text_area, window_manager);
	reg_mouse_listeners(logger, mouse_handler, camera, *text_area, window_manager->get_screen());

	double current_time{window_manager->get_time()};
	double frame_start_time{current_time};
	double last_frame_duration{1.0 / 60.0};

	while (!window_manager->should_close_window()) {

		window_manager->handle_events();
		if (window_manager->is_minimized()) {
			window_manager->wait_events();
			continue;
		}

		// Terrain, and models
		terrain_renderer->draw();
 
		// Controls
		camera.step(static_cast<float>(last_frame_duration));
		
		// GUI
		GUI->draw();

		window_manager->swap_buffers();

		current_time = window_manager->get_time();
		last_frame_duration = current_time - frame_start_time;
		frame_start_time = current_time;

		// Frame stats
		frame_stats.addFrameTime(last_frame_duration);
	}

	return 0;
}

std::tuple<Scene*, TextArea*> create_gui(Camera* camera, FrameStats<512>* frame_stats, Screen screen) {
	Scene* GUI{new Scene()};
	
	Shader* text_shader {
		new Shader(
			"game/shaders/text-vertex.gls",
			"game/shaders/text-fragment.gls"
		)
	};
	BitmapFont* bitmap_font {
		new BitmapFont(
			"game/textures/free-mono-256-4096.tga",
			16,
			16,
			' ',
			0.6f
		)
	};
	BitmapFontRenderer* bitmap_font_renderer{
		new BitmapFontRenderer(
			bitmap_font,
			text_shader,
			static_cast<float>(screen.width) / screen.height
		)
	};

	TextArea* text_area{
		new TextArea(
			bitmap_font_renderer,
			glm::vec2{-1.0f, -1.0f},
			glm::vec2{0.75f, 1.0f},
			0.05f,
			glm::vec3{1.0f, 1.0f, 1.0f}
		)
	};
	GUI->add(text_area);

	FrameStatsDisplay<512>* frame_stats_display{
		new FrameStatsDisplay<512>(
			bitmap_font_renderer,
			frame_stats,
			{-1.0f, 0.85f},
			0.05f
		)
	};
	GUI->add(frame_stats_display);

	CameraStatsDisplay* camera_stats_display{
		new CameraStatsDisplay(
			bitmap_font_renderer,
			camera,
			{-1.0f, 0.95f},
			0.05f
		)
	};
	GUI->add(camera_stats_display);

	return std::make_tuple(GUI, text_area);
}

void reg_kb_listeners(
	std::shared_ptr<Logger> logger,
	std::shared_ptr<KeyboardHandler> keyboard_handler,
	Camera& camera,
	TextArea& text_area,
	std::shared_ptr<WindowManager> window_manager
) {
	keyboard_handler->registerKeyHandler(Input::Key::W, [&camera](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				camera.setDirection(DIRECTION_FORWARD, true);
				break;
			}
			case Input::Action::Release: {
				camera.setDirection(DIRECTION_FORWARD, false);
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	keyboard_handler->registerKeyHandler(Input::Key::A, [&camera](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				camera.setDirection(DIRECTION_LEFT, true);
				break;
			}
			case Input::Action::Release: {
				camera.setDirection(DIRECTION_LEFT, false);
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	keyboard_handler->registerKeyHandler(Input::Key::S, [&camera](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				camera.setDirection(DIRECTION_BACKWARD, true);
				break;
			}
			case Input::Action::Release: {
				camera.setDirection(DIRECTION_BACKWARD, false);
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	keyboard_handler->registerKeyHandler(Input::Key::D, [&camera](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				camera.setDirection(DIRECTION_RIGHT, true);
				break;
			}
			case Input::Action::Release: {
				camera.setDirection(DIRECTION_RIGHT, false);
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	keyboard_handler->registerKeyHandler(Input::Key::LEFT_SHIFT, [&camera](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				camera.setDirection(DIRECTION_UP, true);
				break;
			}
			case Input::Action::Release: {
				camera.setDirection(DIRECTION_UP, false);
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	keyboard_handler->registerKeyHandler(Input::Key::LEFT_CONTROL, [&camera](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				camera.setDirection(DIRECTION_DOWN, true);
				break;
			}
			case Input::Action::Release: {
				camera.setDirection(DIRECTION_DOWN, false);
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	keyboard_handler->registerKeyHandler(Input::Key::ESCAPE, [window_manager](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				window_manager->close_window();
				break;
			}
			case Input::Action::Release: {
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
}

void reg_mouse_listeners(
	std::shared_ptr<Logger> logger,
	std::shared_ptr<MouseHandler> mouse_handler,
	Camera& camera,
	TextArea& text_area,
	Screen screen
) {
	mouse_handler->registerButtonHandler(Input::MouseButton::Left, [&text_area](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				text_area.addLine("Left mouse button pressed!");
				break;
			}
			case Input::Action::Release: {
				break;
			}
			case Input::Action::Repeat: {
				break;
			}
		}
	});
	mouse_handler->registerCursorHandler([&camera, screen](double x_position, double y_position) {
		static double last_x{screen.width / 2.0};
		static double last_y{screen.height / 2.0};

		camera.swipe(x_position - last_x, y_position - last_y);

		last_x = x_position;
		last_y = y_position;
	});
	mouse_handler->registerScrollHandler([&camera](double x_offset, double y_offset) {
		constexpr float SCROLL_FACTOR{0.5f};
		camera.speed += SCROLL_FACTOR * static_cast<float>(y_offset);
	});
}
