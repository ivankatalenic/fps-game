#ifndef OPENGL_TERRAIN_RENDERER_HH
#define OPENGL_TERRAIN_RENDERER_HH

#include "game/headers/renderer/terrain-renderer.hh"

#include "game/headers/renderer/opengl/opengl-drawable-terrain.hh"

#include <memory>

class OpenGLTerrainRenderer : public TerrainRenderer {
public:
	OpenGLTerrainRenderer() = default;

	void init(Screen screen, const Camera* camera) override;
	void addTerrain(std::shared_ptr<Terrain> terrain) override;
	void draw() const override;
private:
	Screen screen_;
	const Camera* camera_;

	std::unique_ptr<OpenGLDrawableTerrain> terrain_;
	Shader shader_;
};

#endif // OPENGL_TERRAIN_RENDERER_HH
