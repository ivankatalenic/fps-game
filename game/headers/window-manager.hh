#pragma once

#include "game/headers/utility/logger.hh"

#define GLFW_INCLUDE_NONE
#include "external/glfw/include/GLFW/glfw3.h"

#include "game/headers/renderer/screen.hh"
#include "game/headers/input/keyboard-handler.hh"
#include "game/headers/input/mouse-handler.hh"

#include <memory>
#include <string>

class WindowManager {
public:
	WindowManager(
		std::shared_ptr<Logger> logger,
		std::shared_ptr<KeyboardHandler> kb_handler,
		std::shared_ptr<MouseHandler> m_handler,
		const std::string& title
	);
	~WindowManager();

	bool should_close_window() const;
	void close_window();
	Screen get_screen() const;
	double get_time() const;
	void swap_buffers() const;
	void handle_events() const;
	void wait_events() const;
	bool is_minimized() const;
private:
	GLFWwindow* window{nullptr};
	GLFWvidmode* video_mode;

	bool is_minimized_{false};

	std::shared_ptr<Logger>          logger;
	std::shared_ptr<KeyboardHandler> keyboard_handler;
	std::shared_ptr<MouseHandler>    mouse_handler;

	void handle_glfw_error() const;
	void setup_callbacks();

	void window_iconify_callback  (GLFWwindow* window, int iconified);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void key_callback             (GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_button_callback    (GLFWwindow* window, int button, int action, int mods);
	void cursor_position_callback (GLFWwindow* window, double xpos, double ypos);
	void scroll_callback          (GLFWwindow* window, double xoffset, double yoffset);
};
