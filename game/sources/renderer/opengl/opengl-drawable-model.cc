#include "game/headers/renderer/opengl/opengl-drawable-model.hh"

OpenGLDrawableModel::OpenGLDrawableModel(std::shared_ptr<Model> model, Shader* shader):
	model_{model}, shader_{shader} {
	
	for (std::shared_ptr<Mesh> mesh : model_->meshes_) {
		meshes_.emplace_back(mesh, shader);
	}
}

void OpenGLDrawableModel::draw() const {
	// Setting model's model matrix
	shader_->setMat4("mat_model", model_->pos_);
	shader_->setMat4("frag_mat_model", model_->pos_);
	
	for (auto& mesh : meshes_) {
		mesh.draw();
	}
}
