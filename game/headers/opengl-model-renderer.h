#ifndef OPENGL_MODEL_RENDERER_H
#define OPENGL_MODEL_RENDERER_H

#include <model-renderer.h>

#include <shader.h>
#include <opengl-drawable-model.h>

class OpenGLModelRenderer : public ModelRenderer {
public:
	OpenGLModelRenderer() = default;

	void init(Screen screen, std::shared_ptr<Camera> camera) override;
	void addModel(std::shared_ptr<Model> model) override;
	void draw() const override;
private:
	Screen screen_;
	std::shared_ptr<Camera> camera_;
	Shader mesh_shader_;

	std::vector<OpenGLDrawableModel> models_;
};

#endif // OPENGL_MODEL_RENDERER_H
