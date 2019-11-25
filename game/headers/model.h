#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <memory>

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/light.h>

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
	Model(const char* path);

	// Methods
	void draw(std::shared_ptr<Shader> shader);
	bool checkCollision(
		glm::vec3 position,
		glm::vec3 step,
		glm::vec3& new_step,
		int recursion_depth = 0
	);

private:
	std::string directory;

	std::vector<Mesh> meshes;
	std::vector<Light> lights;
	std::vector<Texture> loadedTextures;

	// Methods
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(
		aiMaterial* mat,
		aiTextureType type,
		std::string typeName
	);
};

#endif
