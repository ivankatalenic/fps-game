#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>

#include <mesh.h>
#include <shader.h>

struct Light {
	glm::vec3 position;

	glm::vec3 color_ambient;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
};

class Model {
public:
	// Constructors
	Model() = default;
	Model(const std::vector<Mesh>& meshes, const std::vector<Light>& lights);
	Model(Model&& model) noexcept;

	// Methods
	void draw(std::shared_ptr<Shader> shader);
	bool checkCollision(
		glm::vec3 position,
		glm::vec3 step,
		glm::vec3& new_step,
		int recursion_depth = 0
	);
private:
	std::vector<Mesh> _meshes;
	std::vector<Light> _lights;
};

#endif
