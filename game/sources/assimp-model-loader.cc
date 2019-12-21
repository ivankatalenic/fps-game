#include <assimp-model-loader.h>

#include <assimp/Importer.hpp>
#include <assimp/vector3.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>

#include <debug-help.h>
#include <math-aux.h>

AssimpModelLoader::AssimpModelLoader() {

}

Model AssimpModelLoader::loadModel(const std::string& path) {
	const aiScene* scene{
		_importer.ReadFile(
			path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals
		)
	};
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		std::cout << "The model file wasn't read successfully! Error message: \n";
		std::cout << '\t' << _importer.GetErrorString() << std::endl;
		return Model();
	}
	// Process root node
	std::vector<std::shared_ptr<Mesh>> meshes;
	processNode(meshes, scene->mRootNode, scene);
	// Process lights
	std::vector<Light> lights;
	if (scene->HasLights()) {
		debug::print_var(scene->mNumLights, "Model light count");
		for (unsigned int i{0u}; i < scene->mNumLights; i++) {
			const aiLight* ai_light{scene->mLights[i]};
			Light my_light;
			// Setting light-type-specific parameters
			if (ai_light->mType == aiLightSource_POINT) {
				my_light.position = glm::vec3(
					ai_light->mPosition.x,
					ai_light->mPosition.y,
					ai_light->mPosition.z
				);
			} else {
				std::cout << "An unsupported light source has been encountered in the model! Skipping it."
					<< std::endl;
				continue;
			}
			// Setting general light parameters
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
	} // End of light processing
	return Model(meshes, lights);
}

void AssimpModelLoader::processNode(std::vector<std::shared_ptr<Mesh>>& meshes, aiNode* node, const aiScene* scene) {
	// Process all meshes
	for (unsigned int i{0u}; i < node->mNumMeshes; i++) {
		unsigned int mesh_index{node->mMeshes[i]};
		aiMesh* ai_mesh{scene->mMeshes[mesh_index]};
		try {
			meshes.push_back(processMesh(ai_mesh, scene));
		} catch (const char* msg) {
			std::cout << "An error has occurred while loading a mesh: " << msg << std::endl;
		}
	}
	// Process all children nodes
	for (unsigned int i{0u}; i < node->mNumChildren; i++) {
		processNode(meshes, node->mChildren[i], scene);
	}
}

std::shared_ptr<Mesh> AssimpModelLoader::processMesh(aiMesh* mesh, const aiScene* scene) {
	if (!mesh->HasPositions()) {
		throw "Mesh has no vertex positions!";
	}

	// Setting up vertices
	std::vector<Vertex> vertices;
	for (unsigned int i{0u}; i < mesh->mNumFaces; i++) {
		const aiFace ai_face{mesh->mFaces[i]};

		if (ai_face.mNumIndices == 3) {
			glm::vec3 vertices_positions[3];
			glm::vec2 vertices_tex_coords[3];
			for (int j{0}; j < 3; j++) {
				const int vertex_index{static_cast<int>(ai_face.mIndices[j])};

				const aiVector3D ai_vertex{mesh->mVertices[vertex_index]};
				vertices_positions[j] = {ai_vertex.x, ai_vertex.y, ai_vertex.z};
				
				glm::vec2 tex_coords{0.0f, 0.0f};
				// Does a mesh contain texture coordinates?
				if (mesh->mTextureCoords[0] != nullptr) {
					aiVector3D ai_tex_coords{mesh->mTextureCoords[0][vertex_index]};
					tex_coords = {ai_tex_coords.x, ai_tex_coords.y};
				}
				vertices_tex_coords[j] = tex_coords;
			}
			const glm::vec3 cross_product(
				glm::cross(
					vertices_positions[1] - vertices_positions[0],
					vertices_positions[2] - vertices_positions[0]
				)
			);
			glm::vec3 normal(1.0f);
			if (!math_aux::is_zero(glm::length(cross_product))) {
				normal = glm::normalize(cross_product);
			} else {
				std::cout << "Triangle has a normal with length equal to 0!" << std::endl;
			}
			for (unsigned int j{0u}; j < 3; j++) {
				vertices.push_back({vertices_positions[j], normal, vertices_tex_coords[j]});
			}
		} else {
			std::cout << "Found a face that doesn't have three vertices! Skipping it." << std::endl;
		}

	}

	// Setting up textures
	Material material{};
	std::vector<Texture> textures;
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat{scene->mMaterials[mesh->mMaterialIndex]};

		// Setting a material's colors
		aiColor3D ai_color(0.0f, 0.0f, 0.0f);
		mat->Get(AI_MATKEY_COLOR_AMBIENT, ai_color);
		material.color_ambient = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
		material.color_diffuse = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
		mat->Get(AI_MATKEY_COLOR_SPECULAR, ai_color);
		material.color_specular = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
		mat->Get(AI_MATKEY_SHININESS, material.shininess);

		const std::vector<Texture>& diffuse_textures{
			loadMaterialTextures(
				mat,
				aiTextureType_DIFFUSE,
				std::string{"texture_diffuse"}
			)
		};
		for (const Texture& t : diffuse_textures) {
			textures.push_back(t);
		}
		const std::vector<Texture>& specular_textures{
			loadMaterialTextures(
				mat,
				aiTextureType_SPECULAR,
				std::string{"texture_specular"}
			)
		};
		for (const Texture& t : specular_textures) {
			textures.push_back(t);
		}
	}
	
	// Finalizing
	return std::make_shared<Mesh>(vertices, textures, material);
}

std::vector<Texture> AssimpModelLoader::loadMaterialTextures(
	aiMaterial* mat,
	aiTextureType type,
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
