#include "game/headers/model/assimp/assimp-terrain-loader.hh"

#include "external/assimp/include/assimp/vector3.h"
#include "external/assimp/include/assimp/types.h"
#include "external/assimp/include/assimp/postprocess.h"

#include "game/headers/debug-help.hh"
#include "game/headers/math-aux.hh"

#include <stdexcept>

std::shared_ptr<Terrain> AssimpTerrainLoader::loadTerrain(std::string_view path) {
	const std::string filePath{path.data()};
	const aiScene* scene{
		_importer.ReadFile(
			filePath,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals
		)
	};
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		throw std::runtime_error(std::string("cannot load terrain file: ") + std::string(_importer.GetErrorString()));
	}

	// Process root node
	std::vector<std::shared_ptr<Mesh>> meshes;
	processNode(meshes, scene->mRootNode, scene);
	std::vector<std::shared_ptr<Model>> models;
	models.push_back(
		std::make_shared<Model>(
			glm::vec3{0.0f, 0.0f, 0.0f},
			std::move(meshes)
		)
	);

	// Process lights
	std::vector<Light> lights;
	if (scene->HasLights()) {
		processLights(scene, lights);
	}

	return std::make_shared<Terrain>(std::move(models), std::move(lights));
}

void AssimpTerrainLoader::processNode(std::vector<std::shared_ptr<Mesh>>& meshes, aiNode* node, const aiScene* ai_scene) {
	// Process meshes
	for (unsigned int i{0u}; i < node->mNumMeshes; i++) {
		const unsigned int mesh_index{node->mMeshes[i]};
		const aiMesh* ai_mesh{ai_scene->mMeshes[mesh_index]};
		
		meshes.push_back(processMesh(ai_mesh, ai_scene));
	}

	// Process children nodes
	for (unsigned int i{0u}; i < node->mNumChildren; i++) {
		processNode(meshes, node->mChildren[i], ai_scene);
	}
}

std::shared_ptr<Mesh> AssimpTerrainLoader::processMesh(const aiMesh* mesh, const aiScene* scene) {
	if (!mesh->HasPositions()) {
		throw std::runtime_error("the mesh has no vertex positions");
	}

	// Setting up triangles
	std::vector<Triangle> triangles;
	for (unsigned int i{0u}; i < mesh->mNumFaces; i++) {
		const aiFace triangle{mesh->mFaces[i]};

		if (triangle.mNumIndices != 3) {
			// Face is not a triangle, skipping it
			continue;
		}

		triangles.push_back(processTriangle(&triangle, mesh));
	}

	// Setting up a material
	aiMaterial* ai_mat{scene->mMaterials[mesh->mMaterialIndex]};
	Material material{};
	std::vector<Texture> textures;
	processMaterial(ai_mat, material, textures);
	
	return std::make_shared<Mesh>(glm::vec3{0.0f, 0.0f, 0.0f}, std::move(triangles), std::move(textures), material);
}

Triangle AssimpTerrainLoader::processTriangle(const aiFace* ai_triangle, const aiMesh* ai_mesh) {
	Triangle triangle;
	for (int i{0}; i < 3; i++) {
		const int vertex_index{static_cast<int>(ai_triangle->mIndices[i])};

		// Position
		const aiVector3D vertex_vector{ai_mesh->mVertices[vertex_index]};
		triangle.vertices[i].position = {vertex_vector.x, vertex_vector.y, vertex_vector.z};

		// Normal
		if (ai_mesh->HasNormals()) {
			const aiVector3D ai_normal{ai_mesh->mNormals[vertex_index]};

			triangle.vertices[i].normal = {ai_normal.x, ai_normal.y, ai_normal.z};
		}
		
		// Texture
		if (ai_mesh->HasTextureCoords(0)) {
			const aiVector3D ai_tex_coords{ai_mesh->mTextureCoords[0][vertex_index]};

			triangle.vertices[i].tex_coords = {ai_tex_coords.x, ai_tex_coords.y};
		}
	}

	return triangle;
}

void AssimpTerrainLoader::processMaterial(const aiMaterial* ai_mat, Material& material, std::vector<Texture>& textures) {
	// Setting a material's colors
	aiColor3D ai_color(0.0f, 0.0f, 0.0f);

	ai_mat->Get(AI_MATKEY_COLOR_AMBIENT, ai_color);
	material.color_ambient = glm::vec3(ai_color.r, ai_color.g, ai_color.b);

	ai_mat->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
	material.color_diffuse = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
	
	ai_mat->Get(AI_MATKEY_COLOR_SPECULAR, ai_color);
	material.color_specular = glm::vec3(ai_color.r, ai_color.g, ai_color.b);

	ai_mat->Get(AI_MATKEY_SHININESS, material.shininess);

	// Setting up material's colors
	const std::vector<Texture>& diffuse_textures{
		loadMaterialTextures(
			ai_mat,
			aiTextureType_DIFFUSE,
			std::string{"texture_diffuse"}
		)
	};
	for (const Texture& t : diffuse_textures) {
		textures.push_back(t);
	}

	const std::vector<Texture>& specular_textures{
		loadMaterialTextures(
			ai_mat,
			aiTextureType_SPECULAR,
			std::string{"texture_specular"}
		)
	};
	for (const Texture& t : specular_textures) {
		textures.push_back(t);
	}
}

std::vector<Texture> AssimpTerrainLoader::loadMaterialTextures(
	const aiMaterial* mat,
	const aiTextureType type,
	const std::string& type_name
) {
	std::vector<Texture> textures;
	for (unsigned int i{0u}; i < mat->GetTextureCount(type); i++) {
		aiString path;
		if (mat->GetTexture(type, i, &path) == aiReturn_SUCCESS) {
			textures.push_back({type_name, path.C_Str()});
		} else {
			std::cout << "Error: Cannot load a texture!" << std::endl;
		}
	}
	return textures;
}

void AssimpTerrainLoader::processLights(const aiScene* scene, std::vector<Light>& lights) {
	for (unsigned int i{0u}; i < scene->mNumLights; i++) {
		const aiLight* ai_light{scene->mLights[i]};
		if (ai_light->mType != aiLightSource_POINT) {
			continue;
		}

		Light my_light;

		my_light.position = glm::vec3(
			ai_light->mPosition.x,
			ai_light->mPosition.y,
			ai_light->mPosition.z
		);
		my_light.color_ambient = glm::vec3(
			ai_light->mColorAmbient.r,
			ai_light->mColorAmbient.g,
			ai_light->mColorAmbient.b
		);
		my_light.color_diffuse = glm::vec3(
			ai_light->mColorDiffuse.r,
			ai_light->mColorDiffuse.g,
			ai_light->mColorDiffuse.b
		);
		my_light.color_specular = glm::vec3(
			ai_light->mColorSpecular.r,
			ai_light->mColorSpecular.g,
			ai_light->mColorSpecular.b
		);

		lights.push_back(my_light);
	}
}
