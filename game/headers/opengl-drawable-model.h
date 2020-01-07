#ifndef OPENGL_DRAWABLE_MODEL_H
#define OPENGL_DRAWABLE_MODEL_H

#include <drawable.h>

#include <vector>

#include <model.h>
#include <shader.h>
#include <opengl-drawable-mesh.h>

class OpenGLDrawableModel : public Drawable {
public:
	OpenGLDrawableModel(std::shared_ptr<Model> model, Shader& shader);
	void draw() const override;
private:
	std::vector<OpenGLDrawableMesh> meshes_;
};

#endif // OPENGL_DRAWABLE_MODEL_H
