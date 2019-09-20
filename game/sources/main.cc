#include "fps-game-config.h"

#include <fstream>
#include <iostream>
#include <string>
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
void errorCallback(int error, const char* description);
void keyCallback(GLFWwindow* window, 
	int key, int scancode, int action, int mods);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void windowIconifyCallback(GLFWwindow* window, int iconified);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

// Global variables
bool drawing{true};

Shader shaders;
Screen screen;
Camera camera;
Mouse mouse;

// Main function
int main(void) {
	std::cout << "Game version: " << FPS_GAME_VERSION_MAJOR
		<< "." << FPS_GAME_VERSION_MINOR << std::endl;

	// Global variables initialization
	camera = Camera(
		glm::pi<float>() / 2.0f, 						// FOV in radians
		0.1f,											// Near clip plane
		100.0f,											// Far clip plane
		0.0f,											// Yaw: Angle from x-axis
		0.5f * glm::pi<float>(),						// Pitch: Elevation from x-z plane
		glm::vec3(2.0f, 2.0f, 2.0f),					// Position
		2.5f,											// Movement speed
		0.001f											// Mouse sensitivity
	);

	// Setting up GLFW
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) {
		// GLFW initialization failed
		std::cout << "Cannot initialize GLFW library!" << std::endl;
		return -1;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		// Uncomment following two lines for fullscreen
	screen.width = videoMode->width;
	screen.height = videoMode->height;
	// screen.width = 1280;
	// screen.height = 720;
	mouse.lastX = screen.width / 2;
	mouse.lastY = screen.height / 2;

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
	GLFWwindow* window = glfwCreateWindow(screen.width, screen.height, 
		"First GLFW Application", monitor, NULL);
	// GLFWwindow* window = glfwCreateWindow(screen.width, screen.height, "First GLFW Application", NULL, NULL);

	if (!window) {
		// Window creation failed
		glfwTerminate();
		std::cout << "Cannot create a window!" << std::endl;
		return -1;
	}

	// Setting GLFW callbacks
	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetWindowIconifyCallback(window, windowIconifyCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);

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

	// Main render loop
	glfwSwapInterval(0); // VSYNC
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	#define TIMER_WAIT 0.5
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		double frameTime = glfwGetTime();

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
			camera.pos,
			camera.pos + camera.lookAt,
			camera.viewUp
		)};
		const glm::mat4& projectionMatrix{glm::perspective(
			camera.fov,
			(float) screen.width / screen.height,
			camera.clipNear,
			camera.clipFar
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
		
		// Controls
		float currentTime{static_cast<float>(glfwGetTime())};
		camera.step(currentTime - frameTime);

		// Timer
		if ((currentTime - lastTime) >= TIMER_WAIT) {
			lastTime = glfwGetTime();
			double diff{lastTime - frameTime};
			std::cout << "Position: (" << camera.pos.x << ", " 
									   << camera.pos.y << ", " 
									   << camera.pos.z << ")" << '\n';
			std::cout << "FPS: " << 1.0 / diff << std::endl;
		}
	}

	glfwTerminate();
	return 0;
}

void errorCallback(int error, const char* description) {
	std::cout << "Error: " << description << std::endl;
}

void keyCallback(GLFWwindow* window, 
	int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	} else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		camera.setDirection(DIRECTION_FORWARD, true);
	} else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		camera.setDirection(DIRECTION_FORWARD, false);

	} else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		camera.setDirection(DIRECTION_LEFT, true);
	} else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		camera.setDirection(DIRECTION_LEFT, false);

	} else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		camera.setDirection(DIRECTION_BACKWARD, true);
	} else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		camera.setDirection(DIRECTION_BACKWARD, false);

	} else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		camera.setDirection(DIRECTION_RIGHT, true);
	} else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		camera.setDirection(DIRECTION_RIGHT, false);

	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		camera.setDirection(DIRECTION_UP, true);
	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
		camera.setDirection(DIRECTION_UP, false);

	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		camera.setDirection(DIRECTION_DOWN, true);
	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		camera.setDirection(DIRECTION_DOWN, false);
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	screen.width = width;
	screen.height = height;
	glViewport(0, 0, screen.width, screen.height);
}

void windowIconifyCallback(GLFWwindow* window, int iconified) {
	if (iconified) {
		// The window was iconified
		drawing = false;
	} else {
		drawing = true;
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	camera.swipe(xpos - mouse.lastX, ypos - mouse.lastY);
	mouse.lastX = xpos;
	mouse.lastY = ypos;
}
