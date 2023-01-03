#pragma once

#include "game/headers/model/terrain-loader.hh"
#include "game/headers/renderer/terrain-renderer.hh"
#include "game/headers/utility/logger.hh"
#include "game/headers/window-manager.hh"

#include <memory>

class ServiceLocator {
public:
	static ServiceLocator& getInstance();

	std::shared_ptr<TerrainLoader>   getTerrainLoader() const;
	std::shared_ptr<TerrainRenderer> getTerrainRenderer() const;
	std::shared_ptr<Logger>          getLogger() const;
private:
	ServiceLocator() = default;
	~ServiceLocator() = default; // Do not create a custom destructor!

	ServiceLocator(const ServiceLocator&) = default;
	ServiceLocator& operator=(const ServiceLocator&) = default;

	ServiceLocator(ServiceLocator&&) = default;
	ServiceLocator& operator=(ServiceLocator&&) = default;
};
