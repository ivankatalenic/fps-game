#include "game/headers/service-locator.hh"

#include "game/headers/model/assimp/assimp-model-loader.hh"
#include "game/headers/renderer/opengl/opengl-model-renderer.hh"
#include "external/glfw/include/GLFW/glfw3.h"

ServiceLocator& ServiceLocator::getInstance() {
	static ServiceLocator* locator{new ServiceLocator()};
	return *locator;
}

std::unique_ptr<ModelLoader> ServiceLocator::getModelLoader() const {
	return std::make_unique<AssimpModelLoader>();
}

std::unique_ptr<ModelRenderer> ServiceLocator::getModelRenderer() const {
	return std::make_unique<OpenGLModelRenderer>();
}

double ServiceLocator::getCurrentTime() const {
	return glfwGetTime();
}
