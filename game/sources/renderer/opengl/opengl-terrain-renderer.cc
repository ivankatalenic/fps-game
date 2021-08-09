#include "game/headers/renderer/opengl/opengl-terrain-renderer.hh"

#include "game/headers/renderer/opengl/shader.hh"

#include "game/headers/service-locator.hh"

#include "external/glad/glad.h"

#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/type_ptr.hpp"
#include "external/glm/glm/ext/scalar_constants.hpp"
#include "external/glm/glm/ext/matrix_clip_space.hpp"
#include "external/glm/glm/ext/matrix_transform.hpp"

void OpenGLTerrainRenderer::init(Screen screen, const Camera* camera) {
	screen_ = screen;
	camera_ = camera;

	// Setting up OpenGL
	glViewport(0, 0, screen_.width, screen_.height);
	glEnable(GL_DEPTH_TEST);

	// Enable the MSAA
	glEnable(GL_MULTISAMPLE);

	shader_ = Shader("game/shaders/mesh-vertex.gls", "game/shaders/mesh-fragment.gls");
	shader_.use();
}

void OpenGLTerrainRenderer::addTerrain(std::shared_ptr<Terrain> terrain) {
	terrain_ = std::make_unique<OpenGLDrawableTerrain>(terrain, &shader_);
}

void OpenGLTerrainRenderer::draw() const {
	// Clearing the framebuffer
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Common matrices
	const glm::mat4 mat_view{
		glm::lookAt(
			camera_->pos,
			camera_->pos + camera_->lookAt,
			camera_->viewUp
		)
	};
	const glm::mat4 mat_projection{
		glm::perspective(
			camera_->fov,
			static_cast<float>(screen_.width) / static_cast<float>(screen_.height),
			camera_->clipNear,
			camera_->clipFar
		)
	};

	// Setting up projection and view matrices
	shader_.use();
	shader_.setMat4("mat_view", mat_view);
	shader_.setMat4("frag_mat_view", mat_view);
	shader_.setMat4("mat_projection", mat_projection);

	terrain_->draw();
}
