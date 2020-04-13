#ifndef MODEL_HH
#define MODEL_HH

#include "external/glm/glm/glm.hpp"

#include "game/headers/model/mesh.hh"

#include <vector>
#include <memory>

struct Light {
	glm::vec3 position;

	glm::vec3 color_ambient;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
};

class Model {
public:
	std::vector<std::shared_ptr<Mesh>> meshes_;
	std::vector<Light> lights_;

	Model() = default;
	Model(std::vector<std::shared_ptr<Mesh>>&& meshes, std::vector<Light>&& lights);
};

#endif // MODEL_HH
