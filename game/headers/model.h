#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <mesh.h>

struct Light {
	glm::vec3 position;

	glm::vec3 color_ambient;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
};

class Model {
public:
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<Light> lights;

	Model() = default;
	Model(const std::vector<std::shared_ptr<Mesh>>& meshes, const std::vector<Light>& lights);
	
	bool checkCollision(
		glm::vec3 position,
		glm::vec3 step,
		glm::vec3& new_step,
		int recursion_depth = 0
	);
};

#endif
