#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/light.h>

#include <mesh.h>

enum LightType {
	LightType_DIRECTIONAL,		// The Sun
	LightType_POINT,			// A light bulb
	LightType_SPOT,				// A light bulb with a specified maximum light ray angle
	LightType_AMBIENT			// A light that includes all possible reflections from other lights
};

struct Light {
	LightType type;

	glm::vec3 position;
	glm::vec3 direction; 		// For a directional and spot lights

	glm::vec3 colorAmbient;
	glm::vec3 colorDiffuse;
	glm::vec3 colorSpecular;

	float angleInner;			// A spot light has a maximum influence on objects inside this angle
	float angleOuter;			// A spot light has no influence on objects outside of this angle

	float attenuationConstant;	// Constants for calculating the light intesity
	float attenuationLinear;	// on object at a distance d from a light source.
	float attenuationQuadratic;	// lightIntensity = 1 / (attC + attL * d + attQ * d * d)
};

class Model {
public:
	// Constructors
	Model(const char* path);

	// Methods
	void draw(const Shader& shader);
	bool checkCollision(
		glm::vec3 position,
		glm::vec3 step,
		glm::vec3& new_step,
		const Polygon* last_collision_polygon = {nullptr}
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
