#ifndef OPENGL_DRAWABLE_MODEL_HH
#define OPENGL_DRAWABLE_MODEL_HH

#include "game/headers/renderer/drawable.hh"
#include "game/headers/model/model.hh"
#include "game/headers/model/mesh.hh"
#include "game/headers/renderer/opengl/shader.hh"
#include "game/headers/renderer/opengl/opengl-drawable-mesh.hh"

#include <vector>

class OpenGLDrawableModel : public Drawable {
public:
	OpenGLDrawableModel(std::shared_ptr<Model> model, Shader& shader);
	void draw() const override;
private:
	std::vector<OpenGLDrawableMesh> meshes_;
};

#endif // OPENGL_DRAWABLE_MODEL_HH
