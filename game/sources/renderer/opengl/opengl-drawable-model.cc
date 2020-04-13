#include "game/headers/renderer/opengl/opengl-drawable-model.hh"

OpenGLDrawableModel::OpenGLDrawableModel(std::shared_ptr<Model> model, Shader& shader) {
	for (std::shared_ptr<Mesh> mesh : model->meshes_) {
		meshes_.emplace_back(mesh, shader);
	}
}

void OpenGLDrawableModel::draw() const {
	for (OpenGLDrawableMesh mesh : meshes_) {
		mesh.draw();
	}
}