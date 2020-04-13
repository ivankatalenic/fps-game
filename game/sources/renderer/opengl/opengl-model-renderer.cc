#include "game/headers/renderer/opengl/opengl-model-renderer.hh"

#include "game/headers/service-locator.hh"

#include "external/glad/glad.h"

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"
#include "external/glm/glm/ext/scalar_constants.hpp"
#include "external/glm/glm/ext/matrix_clip_space.hpp"
#include "external/glm/glm/ext/matrix_transform.hpp"

void OpenGLModelRenderer::init(Screen screen, const Camera* camera) {
	screen_ = screen;
	camera_ = camera;

	// Setting up OpenGL
	glViewport(0, 0, screen_.width, screen_.height);
	glEnable(GL_DEPTH_TEST);
	
	// Do not render back faces
	// glEnable(GL_CULL_FACE);

	// Enable the MSAA
	glEnable(GL_MULTISAMPLE);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	mesh_shader_ = Shader("game/shaders/mesh-vertex.gls", "game/shaders/mesh-fragment.gls");
	mesh_shader_.use();
	
	mesh_shader_.setVec3("light.color_ambient", glm::vec3(0.5f));
	mesh_shader_.setVec3("light.color_diffuse", glm::vec3(0.5f));
	mesh_shader_.setVec3("light.color_specular", glm::vec3(1.0f));
};

void OpenGLModelRenderer::addModel(std::shared_ptr<Model> model) {
	models_.emplace_back(model, mesh_shader_);
}

void OpenGLModelRenderer::draw() const {
	// Clearing the framebuffer
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Common matrices
	const glm::mat4 mat_model{glm::mat4(1.0f)};
	const glm::mat4 mat_view{
		glm::lookAt(
			camera_->pos,
			camera_->pos + camera_->lookAt,
			camera_->viewUp
		)
	};
	const glm::mat4 mat_projection{glm::perspective(
		camera_->fov,
		static_cast<float>(screen_.width) / screen_.height,
		camera_->clipNear,
		camera_->clipFar
	)};

	// Setting up projection and view matrices
	mesh_shader_.use();
	mesh_shader_.setMat4("mat_model", mat_model);
	mesh_shader_.setMat4("mat_view", mat_view);
	mesh_shader_.setMat4("mat_projection", mat_projection);
	mesh_shader_.setMat4("frag_mat_model", mat_model);
	mesh_shader_.setMat4("frag_mat_view", mat_view);

	// Setting a single light
	constexpr float LIGHT_ROTATION_RADIUS{30.0f};
	constexpr glm::vec3 LIGHT_ROTATION_CENTER{0.0f, 15.0f, 0.0f};
	constexpr float LIGHT_ROTATION_PERIOD{10.0f}; // In seconds
	constexpr float LIGHT_ROTATION_ANGULAR_SPEED{
		2.0f * glm::pi<float>() / LIGHT_ROTATION_PERIOD
	};
	const float light_rotation_angle{
		static_cast<float>(LIGHT_ROTATION_ANGULAR_SPEED * ServiceLocator::getInstance().getCurrentTime())
	};
	mesh_shader_.setVec3(
		"light.position",
		LIGHT_ROTATION_CENTER
			+ glm::vec3(
				LIGHT_ROTATION_RADIUS * cos(light_rotation_angle),
				0.0f,
				LIGHT_ROTATION_RADIUS * sin(light_rotation_angle)
			)
	);

	for (OpenGLDrawableModel model : models_) {
		model.draw();
	}
};
