#include "game/headers/renderer/opengl/opengl-drawable-terrain.hh"

OpenGLDrawableTerrain::OpenGLDrawableTerrain(std::shared_ptr<Terrain> terrain, Shader* shader):
	shader_{shader} {

	if (terrain->lights_.empty()) {
		light_ = {
			{0.0f, 0.0f, 10.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 1.0f}
		};
	} else {
		light_ = terrain->lights_[0];
	}
	
	for (auto& model : terrain->models_) {
		models_.emplace_back(model, shader_);
	}
}
	
void OpenGLDrawableTerrain::draw() const {
	shader_->setVec3("light.position", light_.position);
	shader_->setVec3("light.color_ambient", light_.color_ambient);
	shader_->setVec3("light.color_diffuse", light_.color_diffuse);
	shader_->setVec3("light.color_specular", light_.color_specular);
	
	for (auto& model : models_) {
		model.draw();
	}
}
