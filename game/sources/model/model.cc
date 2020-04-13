#include "game/headers/model/model.hh"

Model::Model(std::vector<std::shared_ptr<Mesh>>&& meshes, std::vector<Light>&& lights):
	meshes_{std::move(meshes)}, lights_{std::move(lights)} {
}
