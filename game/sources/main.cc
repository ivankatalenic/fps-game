#include "fps-game-config.h"

#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Custom classes
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <debug-help.h>

// Constant definitions

// Structure definitions
struct Screen {
	int width;
	int height;
};

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

Shader shaders;
Screen screen;
Mouse mouse;

std::shared_ptr<Camera> camera;

// Main function
int main(void) {
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
		// Uncomment following two lines for fullscreen
	// screen.width = videoMode->width;
	// screen.height = videoMode->height;
	screen.width = 1280;
	screen.height = 720;
	mouse.lastX = screen.width / 2.0;
	mouse.lastY = screen.height / 2.0;

	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
	std::cout << "Refresh rate: " << videoMode->refreshRate << '\n';
	std::cout << "Resolution: " << screen.width << " * " << screen.height << std::endl;
	// GLFWwindow* window = glfwCreateWindow(screen.width, screen.height, 
	// 	"First GLFW Application", monitor, NULL);
	GLFWwindow* window = glfwCreateWindow(
		screen.width, screen.height,
		"daj_strika", 
		NULL, NULL
	);

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

	// Setting up OpenGL
	glViewport(0, 0, screen.width, screen.height);
	glEnable(GL_DEPTH_TEST);

	// Setting up shaders
	shaders = Shader("game/shaders/vertex-shader-source.c", "game/shaders/fragment-shader-source.c");
	shaders.use();

	// Setting up rendering constants
	// Model theModel{"../models/nanosuit/nanosuit.obj"};
	// Model theModel{"../models/custom/Mapa.obj"};
	// Model theModel{"../models/body/DefaultBody.obj"};
	Model theModel{"game/models/plane/plane.obj"};

	camera = std::shared_ptr<Camera>(
		new Camera(
			glm::pi<float>() / 2.0f, 						// FOV in radians
			0.1f,											// Near clip plane
			100.0f,											// Far clip plane
			0.0f,											// Yaw: Angle from x-axis
			0.5f * glm::pi<float>(),						// Pitch: Elevation from x-z plane
			glm::vec3(2.0f, 2.0f, 2.0f),					// Position
			2.0f,											// Movement speed
			0.001f,											// Mouse sensitivity
			theModel
		)
	);

	// Main render loop
	glfwSwapInterval(0); // VSYNC
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

		// Clearing the framebuffer
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Common matrices
		const glm::mat4& viewMatrix{glm::lookAt(
			camera->pos,
			camera->pos + camera->lookAt,
			camera->viewUp
		)};
		const glm::mat4& projectionMatrix{glm::perspective(
			camera->fov,
			(float) screen.width / screen.height,
			camera->clipNear,
			camera->clipFar
		)};

		// Setting up projection and view matrices
		glUniformMatrix4fv(glGetUniformLocation(shaders.id, "view"),
			1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shaders.id, "projection"),
			1, GL_FALSE, glm::value_ptr(projectionMatrix));

		GLint modelLocation{glGetUniformLocation(shaders.id, "model")};

		// Drawing models
		// Model matrix
		glm::mat4 model{glm::mat4(1.0f)};
		// model = glm::scale(model, glm::vec3(0.1f));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		theModel.draw(shaders);

		glfwSwapBuffers(window);
		
		frame_count++;
		fps_sum += 1.0 / last_frame_duration;

		// Controls
		camera->step(static_cast<float>(last_frame_duration));

		// Timer
		constexpr double FPS_DISPLAY_WAIT{1.0};
		if ((current_time - last_fps_display_time) >= FPS_DISPLAY_WAIT) {
			last_fps_display_time = glfwGetTime();

			const double avg_fps{fps_sum / frame_count};
			fps_sum = 0.0;
			frame_count = 0l;
			debug::print_var(avg_fps, "Avg. FPS");
			
			debug::print_vec(camera->pos, "Position");
		}

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
	} else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_FORWARD, false);

	} else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_LEFT, true);
	} else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_LEFT, false);

	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_BACKWARD, true);
	} else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_BACKWARD, false);

	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_RIGHT, true);
	} else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		camera->setDirection(DIRECTION_RIGHT, false);

	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		camera->setDirection(DIRECTION_UP, true);
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
