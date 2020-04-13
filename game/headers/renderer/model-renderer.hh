#ifndef MODEL_RENDERER_HH
#define MODEL_RENDERER_HH

#include "game/headers/renderer/screen.hh"
#include "game/headers/renderer/camera.hh"
#include "game/headers/model/model.hh"

#include <memory>

class ModelRenderer {
public:
	virtual ~ModelRenderer() {};

	virtual void init(Screen screen, const Camera* camera) = 0;
	virtual void addModel(std::shared_ptr<Model> model) = 0;
	virtual void draw() const = 0;
};

#endif // MODEL_RENDERER_HH
