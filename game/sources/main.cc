#include "fps-game-config.h"

// Library classes
#include "external/glad/glad.h"
#include "external/glfw/include/GLFW/glfw3.h"
#include "external/glm/glm/ext/scalar_constants.hpp"

// Private classes
#include "game/headers/service-locator.hh"
#include "game/headers/debug-help.hh"
#include "game/headers/frame-stats.hh"

#include "game/headers/utility/logger.hh"

#include "game/headers/renderer/screen.hh"
#include "game/headers/renderer/camera.hh"
#include "game/headers/renderer/model-renderer.hh"

#include "game/headers/model/model.hh"
#include "game/headers/model/model-loader.hh"

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

// Function prototypes
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, 
	int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_iconify_callback(GLFWwindow* window, int iconified);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Global variables
bool drawing{true};
Screen screen;
KeyboardHandler keyboard_handler;
MouseHandler mouse_handler;
std::unique_ptr<Logger> logger{ServiceLocator::getInstance().getLogger()};

int main() {
	// Setting up GLFW
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		// GLFW initialization failed
		logger->Error("cannot initialize the GLFW library");
		return -1;
	}

	GLFWmonitor* monitor{glfwGetPrimaryMonitor()};
	const GLFWvidmode* videoMode{glfwGetVideoMode(monitor)};
	screen.width = videoMode->width;
	screen.height = videoMode->height;

	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Multisample Anti-Aliasing (MSAA) number of samples
	glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
	logger->Info("Refresh rate: " + std::to_string(videoMode->refreshRate));
	logger->Info("Resolution: " + std::to_string(screen.width) + " * " + std::to_string(screen.height));
	GLFWwindow* window = glfwCreateWindow(screen.width, screen.height, 
		"fps game", monitor, NULL);

	if (!window) {
		// Window creation failed
		glfwTerminate();
		logger->Error("cannot create a window");
		return -1;
	}

	// Setting GLFW callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowIconifyCallback(window, window_iconify_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) {
	    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		logger->Info("raw mouse motion is enabled");
	} else {
		logger->Info("raw mouse motion is not supported");
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		logger->Info("cannot initialize the GLAD library");
		glfwTerminate();
		return -1;
	}

	glfwGetFramebufferSize(window, &screen.width, &screen.height);


	// Setting up a 3D camera
	Camera camera(
		glm::pi<float>() / 2.0f, 		// FOV in radians
		0.1f,							// Near clip plane
		500.0f,							// Far clip plane
		0.0f,							// Orientation Yaw: Angle from x-axis
		0.5f * glm::pi<float>(),		// Orientation Pitch: Elevation from x-z plane
		glm::vec3(2.0f, 2.0f, 2.0f),	// Position
		4.0f,							// Movement speed
		0.001f							// Mouse sensitivity
	);


	// Setting the GUI
	Scene GUI;
	// Setting up a UI renderer
	Shader text_shader("game/shaders/text-vertex.gls", "game/shaders/text-fragment.gls");
	BitmapFont bitmap_font("game/textures/free-mono-256-4096.tga", 16, 16, ' ', 0.6f);
	BitmapFontRenderer bitmap_font_renderer(bitmap_font, text_shader, static_cast<float>(screen.width) / screen.height);

	TextArea text_area(
		bitmap_font_renderer,
		glm::vec2{-1.0f, -1.0f},
		glm::vec2{0.75f, 1.0f},
		0.05f,
		glm::vec3{1.0f, 1.0f, 1.0f}
	);
	GUI.add(&text_area);

	FrameStats<512> frame_stats;
	FrameStatsDisplay<512> frame_stats_display(bitmap_font_renderer, frame_stats, {-1.0f, 0.85f}, 0.05f);
	GUI.add(&frame_stats_display);

	CameraStatsDisplay camera_stats_display(bitmap_font_renderer, camera, {-1.0f, 0.95f}, 0.05f);
	GUI.add(&camera_stats_display);

	// Setting up a model loader, and a model renderer
	std::unique_ptr<ModelRenderer> model_renderer{ServiceLocator::getInstance().getModelRenderer()};
	model_renderer->init(screen, &camera);
	std::unique_ptr<ModelLoader> model_loader{ServiceLocator::getInstance().getModelLoader()};
	model_renderer->addModel(model_loader->loadModel("game/terrains/plane-cube/plane-cube.obj"));

	// Setting up inputs
	keyboard_handler.registerKeyHandler(Input::Key::W, [&](Input::Action action, Input::Modifier modifier) {
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
	keyboard_handler.registerKeyHandler(Input::Key::A, [&](Input::Action action, Input::Modifier modifier) {
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
	keyboard_handler.registerKeyHandler(Input::Key::S, [&](Input::Action action, Input::Modifier modifier) {
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
	keyboard_handler.registerKeyHandler(Input::Key::D, [&](Input::Action action, Input::Modifier modifier) {
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
	keyboard_handler.registerKeyHandler(Input::Key::LEFT_SHIFT, [&](Input::Action action, Input::Modifier modifier) {
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
	keyboard_handler.registerKeyHandler(Input::Key::LEFT_CONTROL, [&](Input::Action action, Input::Modifier modifier) {
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
	keyboard_handler.registerKeyHandler(Input::Key::ESCAPE, [&](Input::Action action, Input::Modifier modifier) {
		switch (action) {
			case Input::Action::Press: {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
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
	mouse_handler.registerButtonHandler(Input::MouseButton::Left, [&](Input::Action action, Input::Modifier modifier) {
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
	mouse_handler.registerCursorHandler([&](double x_position, double y_position) {
		static double last_x{screen.width / 2.0};
		static double last_y{screen.height / 2.0};

		camera.swipe(x_position - last_x, y_position - last_y);

		last_x = x_position;
		last_y = y_position;
	});
	mouse_handler.registerScrollHandler([&](double x_offset, double y_offset) {
		constexpr float SCROLL_FACTOR{0.5f};
		camera.speed += SCROLL_FACTOR * static_cast<float>(y_offset);
	});


	// Main render loop
	glfwSwapInterval(0); // VSYNC

	double current_time{glfwGetTime()};
	double frame_start_time{current_time};
	double last_frame_duration{1.0 / 60.0};

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		if (!drawing) {
			glfwWaitEvents();
			continue;
		}

		// Terrain, and models
		model_renderer->draw();

		// Controls
		camera.step(static_cast<float>(last_frame_duration));
		
		// GUI
		GUI.draw();

		glfwSwapBuffers(window);

		current_time = glfwGetTime();
		last_frame_duration = current_time - frame_start_time;
		frame_start_time = current_time;

		// Frame stats
		frame_stats.addFrameTime(last_frame_duration);
	}

	glfwTerminate();
	return 0;
}

void error_callback(int error, const char* description) {
	logger->Error(description);
}

void key_callback(GLFWwindow* window, 
	int key, int scancode, int action, int mods) {
	keyboard_handler.processKey(Input::Key{key}, Input::Action{action}, Input::Modifier{mods});
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	screen.width = width;
	screen.height = height;
	glViewport(0, 0, screen.width, screen.height);
}

void window_iconify_callback(GLFWwindow* window, int iconified) {
	if (iconified) {
		// The window was iconified
		drawing = false;
	} else {
		drawing = true;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	mouse_handler.processButton(Input::MouseButton{button}, Input::Action{action}, Input::Modifier{mods});
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_handler.processCursor(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mouse_handler.processScroll(xoffset, yoffset);
}
