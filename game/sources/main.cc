#include "fps-game-config.h"

// System classes
#include <iostream>
#include <string>
#include <memory>

// Library classes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/scalar_constants.hpp>

// Private classes
#include <service-locator.h>
#include <screen.h>
#include <camera.h>
#include <model.h>
#include <model-loader.h>
#include <model-renderer.h>
#include <debug-help.h>
#include <bitmap-font.h>
#include <bitmap-font-renderer.h>
#include <text-area.h>

// Structure definitions
struct Mouse {
	double lastX;
	double lastY;
};

// Function prototypes
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, 
	int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_iconify_callback(GLFWwindow* window, int iconified);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Global variables
bool drawing{true};
Screen screen;
Mouse mouse;
std::shared_ptr<Camera> camera;
TextArea* text_a;

int main() {
	std::cout << "Game version: " << FPS_GAME_VERSION_MAJOR
		<< "." << FPS_GAME_VERSION_MINOR << std::endl;

	// Setting up GLFW
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		// GLFW initialization failed
		std::cout << "Cannot initialize GLFW library!" << std::endl;
		return -1;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
	screen.width = videoMode->width;
	screen.height = videoMode->height;
	mouse.lastX = screen.width / 2.0;
	mouse.lastY = screen.height / 2.0;

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
	std::cout << "Refresh rate: " << videoMode->refreshRate << '\n';
	std::cout << "Resolution: " << screen.width << " * " << screen.height << std::endl;
	GLFWwindow* window = glfwCreateWindow(screen.width, screen.height, 
		"fps game", monitor, NULL);

	if (!window) {
		// Window creation failed
		glfwTerminate();
		std::cout << "Cannot create a window!" << std::endl;
		return -1;
	}

	// Setting GLFW callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowIconifyCallback(window, window_iconify_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) {
	    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		std::cout << "Raw mouse motion is enabled!" << std::endl;
	} else {
		std::cout << "Raw mouse motion is not supported!" << std::endl;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Cannot initilize GLAD library!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwGetFramebufferSize(window, &screen.width, &screen.height);


	// Setting up a 3D camera
	camera = std::make_shared<Camera>(
		glm::pi<float>() / 2.5f, 						// FOV in radians
		0.1f,											// Near clip plane
		100.0f,											// Far clip plane
		0.0f,											// Yaw: Angle from x-axis
		0.5f * glm::pi<float>(),						// Pitch: Elevation from x-z plane
		glm::vec3(2.0f, 2.0f, 2.0f),					// Position
		4.0f,											// Movement speed
		0.001f											// Mouse sensitivity
	);


	// Setting up a UI renderer
	Shader text_shader("game/shaders/text-vertex.c", "game/shaders/text-fragment.c");
	BitmapFont font("game/textures/free-mono-256-4096.tga", 16, 16, ' ', 0.6f);
	BitmapFontRenderer text_renderer(font, text_shader, static_cast<float>(screen.width) / screen.height);
	TextArea text_area(text_renderer, {-1.0f, -1.0f}, {0.75f, 1.0f}, 0.05f, {1.0f, 1.0f, 1.0f});
	text_a = &text_area;


	// Setting up a model loader, and a model renderer
	std::unique_ptr<ModelRenderer> model_renderer{ServiceLocator::getInstance().getModelRenderer()};
	model_renderer->init(screen, camera);
	std::unique_ptr<ModelLoader> model_loader{ServiceLocator::getInstance().getModelLoader()};
	model_renderer->addModel(model_loader->loadModel("game/models/plane-cube/plane-cube.obj"));

	
	// Main render loop
	glfwSwapInterval(0); // VSYNC

	long frame_count{0l};
	double fps_sum{0.0};

	double current_time{glfwGetTime()};
	double frame_start_time{current_time};
	double last_frame_duration{1.0 / 60.0};

	double last_fps_display_time{current_time};

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		if (!drawing) {
			glfwWaitEvents();
			continue;
		}

		model_renderer->draw();

		// Controls
		camera->step(static_cast<float>(last_frame_duration));


		// Timer, and the FPS display
		frame_count++;
		fps_sum += 1.0 / last_frame_duration;
		constexpr double FPS_DISPLAY_WAIT{1.0};
		constexpr bool SHOW_FPS{true};
		double avg_fps;
		if (SHOW_FPS
				&& (current_time - last_fps_display_time) >= FPS_DISPLAY_WAIT) {
			last_fps_display_time = glfwGetTime();

			avg_fps = fps_sum / frame_count;
			fps_sum = 0.0;
			frame_count = 0l;
		}
		
		text_renderer.draw("Avg. FPS: " + std::to_string(avg_fps), 0.05f, glm::vec2(-1.0f, 0.95f), glm::vec3(1.0f));
		text_renderer.draw("Position: (" + std::to_string(camera->pos.x)
				+ ", " + std::to_string(camera->pos.y) + ", " + std::to_string(camera->pos.z) + ")",
			0.05f, glm::vec2(-1.0f, 0.9f), glm::vec3(1.0f));
		text_area.draw();
		glfwSwapBuffers(window);


		current_time = glfwGetTime();
		last_frame_duration = current_time - frame_start_time;
		frame_start_time = current_time;
	}

	glfwTerminate();
	return 0;
}

void error_callback(int error, const char* description) {
	std::cout << "Error: " << description << std::endl;
}

void key_callback(GLFWwindow* window, 
	int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	} else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_FORWARD, true);
		text_a->addLine("W pressed");
	} else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_FORWARD, false);

	} else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_LEFT, true);
	} else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_LEFT, false);

	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_BACKWARD, true);
		text_a->addLine("S pressed");
	} else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_BACKWARD, false);

	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_RIGHT, true);
	} else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_RIGHT, false);

	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_UP, true);
		text_a->addLine("Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in");
	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_UP, false);

	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_DOWN, true);
	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_DOWN, false);
	}
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

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	camera->swipe(xpos - mouse.lastX, ypos - mouse.lastY);
	mouse.lastX = xpos;
	mouse.lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	constexpr float SCROLL_FACTOR{0.5f};
	camera->speed += SCROLL_FACTOR * static_cast<float>(yoffset);
}
