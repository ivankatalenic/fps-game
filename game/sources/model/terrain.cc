#include "game/headers/model/terrain.hh"

#include <memory>
#include <vector>

Terrain::Terrain(std::vector<std::shared_ptr<Model>>&& models, std::vector<Light>&& lights):
	models_{std::move(models)}, lights_{std::move(lights)} {

}
