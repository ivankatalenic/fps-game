#ifndef ASSIMP_TERRAIN_LOADER_HH
#define ASSIMP_TERRAIN_LOADER_HH

#include "game/headers/model/terrain-loader.hh"

#include "external/assimp/include/assimp/mesh.h"
#include "external/assimp/include/assimp/scene.h"
#include "external/assimp/include/assimp/material.h"
#include "external/assimp/include/assimp/texture.h"
#include "external/assimp/include/assimp/Importer.hpp"

#include "game/headers/model/model.hh"
#include "game/headers/model/mesh.hh"

#include <memory>

class AssimpTerrainLoader : public TerrainLoader {
public:
	AssimpTerrainLoader() = default;

	virtual std::shared_ptr<Terrain> loadTerrain(std::string_view filePath) override;
private:
	Assimp::Importer _importer;
    std::vector<Texture> _loaded_textures;

    void processNode(std::vector<std::shared_ptr<Mesh>>& meshes, aiNode* node, const aiScene* scene);
    std::shared_ptr<Mesh> processMesh(const aiMesh* mesh, const aiScene* scene);
	Triangle processTriangle(const aiFace* ai_triangle, const aiMesh* ai_mesh);
	void processMaterial(const aiMaterial* ai_mat, Material& material, std::vector<Texture>& textures);
	void processLights(const aiScene* scene, std::vector<Light>& lights);

    std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, const aiTextureType type, const std::string& type_name);
};

#endif // ASSIMP_TERRAIN_LOADER_HH
