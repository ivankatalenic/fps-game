#ifndef TERRAIN_RENDERER_HH
#define TERRAIN_RENDERER_HH

#include "game/headers/renderer/screen.hh"
#include "game/headers/renderer/camera.hh"
#include "game/headers/model/terrain.hh"

#include <memory>

class TerrainRenderer {
public:
	virtual ~TerrainRenderer() {};

	virtual void init(Screen screen, const Camera* camera) = 0;
	virtual void addTerrain(std::shared_ptr<Terrain> model) = 0;
	virtual void draw() const = 0;
};

#endif // TERRAIN_RENDERER_HH
