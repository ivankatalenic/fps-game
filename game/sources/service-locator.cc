#include "game/headers/service-locator.hh"

#include "game/headers/model/assimp/assimp-terrain-loader.hh"
#include "game/headers/renderer/opengl/opengl-terrain-renderer.hh"
#include "game/headers/utility/logger.hh"
#include "game/headers/utility/console-logger.hh"
#include "external/glfw/include/GLFW/glfw3.h"

ServiceLocator& ServiceLocator::getInstance() {
	static ServiceLocator* locator{new ServiceLocator()};
	return *locator;
}

std::unique_ptr<TerrainLoader> ServiceLocator::getTerrainLoader() const {
	return std::make_unique<AssimpTerrainLoader>();
}

std::unique_ptr<TerrainRenderer> ServiceLocator::getTerrainRenderer() const {
	return std::make_unique<OpenGLTerrainRenderer>();
}

std::unique_ptr<Logger> ServiceLocator::getLogger() const {
	return std::make_unique<ConsoleLogger>();
}

double ServiceLocator::getCurrentTime() const {
	return glfwGetTime();
}
