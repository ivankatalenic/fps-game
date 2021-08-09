#ifndef SERVICE_LOCATOR_HH
#define SERVICE_LOCATOR_HH

#include "game/headers/model/terrain-loader.hh"
#include "game/headers/renderer/terrain-renderer.hh"
#include "game/headers/utility/logger.hh"

#include <memory>

class ServiceLocator {
public:
	static ServiceLocator& getInstance();

	std::unique_ptr<TerrainLoader> getTerrainLoader() const;
	std::unique_ptr<TerrainRenderer> getTerrainRenderer() const;
	std::unique_ptr<Logger> getLogger() const;
	double getCurrentTime() const;
private:
	ServiceLocator() = default;
	~ServiceLocator() = default; // Do not create a custom destructor!

	ServiceLocator(const ServiceLocator&) = default;
	ServiceLocator& operator=(const ServiceLocator&) = default;

	ServiceLocator(ServiceLocator&&) = default;
	ServiceLocator& operator=(ServiceLocator&&) = default;
};

#endif // SERVICE_LOCATOR_HH
