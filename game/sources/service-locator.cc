#include "game/headers/service-locator.hh"

#include "game/headers/model/assimp/assimp-terrain-loader.hh"
#include "game/headers/renderer/opengl/opengl-terrain-renderer.hh"
#include "game/headers/utility/logger.hh"
#include "game/headers/utility/console-logger.hh"
#include "game/headers/window-manager.hh"

ServiceLocator& ServiceLocator::getInstance() {
	static ServiceLocator* locator{new ServiceLocator()};
	return *locator;
}

std::shared_ptr<TerrainLoader> ServiceLocator::getTerrainLoader() const {
	return std::make_shared<AssimpTerrainLoader>();
}

std::shared_ptr<TerrainRenderer> ServiceLocator::getTerrainRenderer() const {
	return std::make_shared<OpenGLTerrainRenderer>();
}

std::shared_ptr<Logger> ServiceLocator::getLogger() const {
	return std::make_shared<ConsoleLogger>();
}
