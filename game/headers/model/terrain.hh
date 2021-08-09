#ifndef TERRAIN_HH
#define TERRAIN_HH

#include "external/glm/glm/glm.hpp"

#include "game/headers/model/model.hh"

#include <vector>
#include <memory>

struct Light {
	glm::vec3 position;

	glm::vec3 color_ambient;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
};

class Terrain {
public:
	std::vector<std::shared_ptr<Model>> models_;
	std::vector<Light> lights_;

	Terrain(std::vector<std::shared_ptr<Model>>&& models, std::vector<Light>&& lights);
};

#endif // TERRAIN_HH
