#ifndef ASSIMP_MODEL_LOADER_H
#define ASSIMP_MODEL_LOADER_H

#include <model-loader.h>

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/texture.h>
#include <assimp/Importer.hpp>

class AssimpModelLoader : public ModelLoader {
public:
    AssimpModelLoader();
    Model loadModel(const std::string& path) override;
private:
    Assimp::Importer _importer;
    std::vector<Texture> _loaded_textures;

    void processNode(std::vector<Mesh>& meshes, aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& type_name);
};

#endif // ASSIMP_MODEL_LOADER_H
