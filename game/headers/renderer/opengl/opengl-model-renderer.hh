#ifndef OPENGL_MODEL_RENDERER_HH
#define OPENGL_MODEL_RENDERER_HH

#include "game/headers/renderer/model-renderer.hh"
#include "game/headers/renderer/opengl/opengl-drawable-model.hh"

class OpenGLModelRenderer : public ModelRenderer {
public:
	OpenGLModelRenderer() = default;

	void init(Screen screen, const Camera* camera) override;
	void addModel(std::shared_ptr<Model> model) override;
	void draw() const override;
private:
	Screen screen_;
	const Camera* camera_;
	Shader mesh_shader_;

	std::vector<OpenGLDrawableModel> models_;
};

#endif // OPENGL_MODEL_RENDERER_HH
