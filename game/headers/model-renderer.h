#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <memory.h>

#include <screen.h>
#include <camera.h>
#include <model.h>

class ModelRenderer {
public:
	virtual ~ModelRenderer() {};

	virtual void init(Screen screen, std::shared_ptr<Camera> camera) = 0;
	virtual void addModel(std::shared_ptr<Model> model) = 0;
	virtual void draw() const = 0;
};

#endif // MODEL_RENDERER_H
