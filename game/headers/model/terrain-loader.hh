#ifndef TERRAIN_LOADER_HH
#define TERRAIN_LOADER_HH

#include "game/headers/model/terrain.hh"

#include <memory>
#include <string_view>

class TerrainLoader {
public:
	virtual ~TerrainLoader() {};
	virtual std::shared_ptr<Terrain> loadTerrain(std::string_view filePath) = 0;
};

#endif // TERRAIN_LOADER_HH
