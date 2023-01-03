#include "game/headers/window-manager.hh"

#include "external/glad/glad.h"

#include <stdexcept>

void WindowManager::handle_glfw_error() const {
	const char* error_desc;
	const int code{glfwGetError(&error_desc)};
	if (code != GLFW_NO_ERROR) {
		logger->Error("failed to initialize GLFW: " + std::string(error_desc));
		throw std::runtime_error("can't initialize GLFW library");
	}
}

WindowManager::WindowManager(
	std::shared_ptr<Logger> logger_,
	std::shared_ptr<KeyboardHandler> kb_handler_,
	std::shared_ptr<MouseHandler> m_handler_,
	const std::string& title
): logger{logger_}, keyboard_handler{kb_handler_}, mouse_handler{m_handler_} {
	if (!glfwInit()) {
		handle_glfw_error();
	}

	GLFWmonitor* monitor{glfwGetPrimaryMonitor()};
	if (monitor == nullptr) {
		handle_glfw_error();
	}
	video_mode = const_cast<GLFWvidmode*>(glfwGetVideoMode(monitor));

	glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Multisample Anti-Aliasing (MSAA) number of samples
	glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

	logger->Info("Refresh rate: " + std::to_string(video_mode->refreshRate));
	logger->Info("Resolution: " + std::to_string(video_mode->width) + " * " + std::to_string(video_mode->height));
	window = glfwCreateWindow(
		video_mode->width,
		video_mode->height,
		title.c_str(),
		monitor,
		NULL
	);
	if (!window) {
		handle_glfw_error();
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		logger->Info("cannot initialize the GLAD library");
		throw std::runtime_error("cannot initialize the GLAD library");
	}

	// VSYNC: 0 = off, 1 = on
	glfwSwapInterval(0);

	// Hide the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Enable the raw mouse motion
	if (glfwRawMouseMotionSupported()) {
	    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		logger->Info("Raw mouse motion is enabled");
	} else {
		logger->Info("Raw mouse motion is not supported");
	}

	setup_callbacks();
}

void WindowManager::setup_callbacks() {
	static WindowManager* ptr{this};
	glfwSetWindowIconifyCallback(
		window,
		[](GLFWwindow* window, int iconified) {
			ptr->window_iconify_callback(window, iconified);
		}
	);
	glfwSetFramebufferSizeCallback(
		window,
		[](GLFWwindow* window, int width, int height) {
			ptr->framebuffer_size_callback(window, width, height);
		}
	);
	glfwSetKeyCallback(
		window,
		[](GLFWwindow* window, int key, int scancode, int action, int mods) {
			ptr->key_callback(window, key, scancode, action, mods);
		}
	);
	glfwSetCursorPosCallback(
		window,
		[](GLFWwindow* window, double xpos, double ypos) {
			ptr->cursor_position_callback(window, xpos, ypos);
		}
	);
	glfwSetMouseButtonCallback(
		window,
		[](GLFWwindow* window, int button, int action, int mods) {
			ptr->mouse_button_callback(window, button, action, mods);
		}
	);
	glfwSetScrollCallback(
		window,
		[](GLFWwindow* window, double xoffset, double yoffset) {
			ptr->scroll_callback(window, xoffset, yoffset);
		}
	);
}

void WindowManager::window_iconify_callback(GLFWwindow* window, int iconified) {
	is_minimized_ = iconified;
}

void WindowManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
}

void WindowManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	keyboard_handler->processKey(Input::Key{key}, Input::Action{action}, Input::Modifier{mods});
}

void WindowManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	mouse_handler->processButton(Input::MouseButton{button}, Input::Action{action}, Input::Modifier{mods});
}

void WindowManager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	mouse_handler->processCursor(xpos, ypos);
}

void WindowManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	mouse_handler->processScroll(xoffset, yoffset);
}

WindowManager::~WindowManager() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool WindowManager::should_close_window() const {
	const int should_close{glfwWindowShouldClose(window)};
	handle_glfw_error();
	return should_close == GLFW_TRUE;
};

void WindowManager::close_window() {
	glfwSetWindowShouldClose(window, GLFW_TRUE);
	handle_glfw_error();
}

Screen WindowManager::get_screen() const {
	Screen screen;
	glfwGetFramebufferSize(window, &screen.width, &screen.height);
	handle_glfw_error();
	return screen;
}

double WindowManager::get_time() const {
	const double time{glfwGetTime()};
	if (time == 0.0) {
		handle_glfw_error();
	}
	return time;
}

void WindowManager::swap_buffers() const {
	glfwSwapBuffers(window);
	handle_glfw_error();
}

void WindowManager::handle_events() const {
	glfwPollEvents();
}

void WindowManager::wait_events() const {
	glfwWaitEvents();
}

bool WindowManager::is_minimized() const {
	return is_minimized_;
}
