#ifndef OPENGL_DRAWABLE_TERRAIN_HH
#define OPENGL_DRAWABLE_TERRAIN_HH

#include "game/headers/renderer/drawable.hh"
#include "game/headers/model/terrain.hh"
#include "game/headers/renderer/opengl/shader.hh"
#include "game/headers/renderer/opengl/opengl-drawable-model.hh"

#include <memory>
#include <vector>

class OpenGLDrawableTerrain : public Drawable {
public:
	OpenGLDrawableTerrain(std::shared_ptr<Terrain> terrain, Shader* shader);
	
	void draw() const override;
private:
	Shader* shader_;
	Light light_;
	std::vector<OpenGLDrawableModel> models_;
};

#endif // OPENGL_DRAWABLE_TERRAIN_HH
