#include <model.h>

#include <assimp/Importer.hpp>
#include <assimp/vector3.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <glm/glm.hpp>

unsigned int loadTextureFromFile(std::string file, std::string directory);

Model::Model(const char* path) {
	loadModel(std::string(path));
}

void Model::draw(const Shader& shader) {
	for (auto& mesh : meshes) {
		mesh.draw(shader);
	}
}

void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		std::cout << "The model file wasn't read successfully! Error message: \n";
		std::cout << '\t' << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	// Process root node
	processNode(scene->mRootNode, scene);
	// Process lights
	if (scene->HasLights()) {
		for (unsigned int i{0u}; i < scene->mNumLights; i++) {
			const aiLight* light{scene->mLights[i]};
			Light theLight;
			// Setting light-type-specific parameters
			switch (light->mType) {
				case aiLightSource_DIRECTIONAL:
					theLight.direction = glm::vec3{
						light->mDirection.x,
						light->mDirection.y,
						light->mDirection.z
					};
					break;
				case aiLightSource_POINT:
					theLight.position = glm::vec3{
						light->mPosition.x,
						light->mPosition.y,
						light->mPosition.z
					};
					theLight.attenuationConstant = light->mAttenuationConstant;
					theLight.attenuationLinear = light->mAttenuationLinear;
					theLight.attenuationQuadratic = light->mAttenuationQuadratic;
					break;
				case aiLightSource_SPOT:
					theLight.position = glm::vec3{
						light->mPosition.x,
						light->mPosition.y,
						light->mPosition.z
					};
					theLight.direction = glm::vec3{
						light->mDirection.x,
						light->mDirection.y,
						light->mDirection.z
					};
					theLight.angleInner = light->mAngleInnerCone;
					theLight.angleOuter = light->mAngleOuterCone;

					theLight.attenuationConstant = light->mAttenuationConstant;
					theLight.attenuationLinear = light->mAttenuationLinear;
					theLight.attenuationQuadratic = light->mAttenuationQuadratic;
					break;
				case aiLightSource_AMBIENT:
					break;
				default:
					std::cout << "An unsupported light source has been encountered in the model! Skipping it."
							  << std::endl;
					continue;
			}
			// Setting general light parameters
			theLight.colorAmbient = glm::vec3(
				light->mColorAmbient.r,
				light->mColorAmbient.g,
				light->mColorAmbient.b
			);
			theLight.colorDiffuse = glm::vec3(
				light->mColorDiffuse.r,
				light->mColorDiffuse.g,
				light->mColorDiffuse.b
			);
			theLight.colorSpecular = glm::vec3(
				light->mColorSpecular.r,
				light->mColorSpecular.g,
				light->mColorSpecular.b
			);
			lights.push_back(theLight);
		}
	} else {
		// Add one directional (Sun) light
		Light theLight;
		theLight.colorAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
		theLight.colorDiffuse = glm::vec3(0.99216f, 0.72157f, 0.07451f);
		theLight.colorSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
		theLight.direction = glm::normalize(glm::vec3(0.3f, -0.8f, 0.0f));
		lights.push_back(theLight);
	} // End of light processing
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// Process all meshes
	for (unsigned int i{0u}; i < node->mNumMeshes; i++) {
		unsigned int meshIndex{node->mMeshes[i]};
		aiMesh* theMesh{scene->mMeshes[meshIndex]};
		try {
			Mesh newMesh{processMesh(theMesh, scene)};
			meshes.push_back(newMesh);
		} catch (const char* msg) {
			std::cout << "An error has occurred while loading a mesh: " << msg << std::endl;
		}
	}
	// Process all children nodes
	for (unsigned int i{0u}; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	// TODO Throw exceptions if a mesh creation fails (for example: if there are no vertices present in the mesh)
	// Setting up vertices
	if (!mesh->HasPositions()) {
		throw "Mesh has no vertex positions!";
	}
	if (!mesh->HasNormals()) {
		throw "Mesh has no vertex normals!";
	}

	std::vector<Vertex> vertices;
	for (unsigned int i{0u}; i < mesh->mNumVertices; i++) {
		aiVector3D theVertex{mesh->mVertices[i]};
		glm::vec3 position{theVertex.x, theVertex.y, theVertex.z};

		aiVector3D theNormal{mesh->mNormals[i]};
		glm::vec3 normal{theNormal.x, theNormal.y, theNormal.z};

		glm::vec2 texCoords{0.0f, 0.0f};
		// Does a mesh contain texture coordinates?
		if (mesh->mTextureCoords[0] != nullptr) {
			aiVector3D tex{mesh->mTextureCoords[0][i]};
			texCoords = glm::vec2{tex.x, tex.y};
		}

		Vertex newVertex{
			position,
			normal,
			texCoords
		};
		vertices.push_back(newVertex);
	}

	// Setting up indices
	std::vector<unsigned int> indices;
	for (unsigned int i{0u}; i < mesh->mNumFaces; i++) {
		aiFace face{mesh->mFaces[i]};
		if (face.mNumIndices != 3u) {
			std::cout << "Found a face that doesn't have three vertices! Skipping it." << std::endl;
			continue;
		}
		for (unsigned int j{0u}; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	// Setting up textures
	// Does a mesh contain a material?
	std::vector<Texture> textures;
	if (mesh->mMaterialIndex >= 0) {
		const std::vector<Texture>& diffuseTextures{
			loadMaterialTextures(
				scene->mMaterials[mesh->mMaterialIndex],
				aiTextureType_DIFFUSE,
				std::string{"texture_diffuse"}
			)
		};
		for (const Texture& t : diffuseTextures) {
			textures.push_back(t);
		}
		const std::vector<Texture>& specularTextures{
			loadMaterialTextures(
				scene->mMaterials[mesh->mMaterialIndex],
				aiTextureType_SPECULAR,
				std::string{"texture_specular"}
			)
		};
		for (const Texture& t : specularTextures) {
			textures.push_back(t);
		}
	}
	
	// Finalizing
	return Mesh{vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(
	aiMaterial* mat,
	aiTextureType type,
	std::string typeName
) {
	std::vector<Texture> textures;
	for (unsigned int i{0u}; i < mat->GetTextureCount(type); i++) {
		aiString path;
		if (mat->GetTexture(type, i, &path) == aiReturn_SUCCESS) {
			// unsigned int textureId{loadTextureFromFile(std::string{path.C_Str()}, directory)};
			// textures.push_back(Texture(textureId, typeName, std::string{path.C_Str()}));
			// Check if a texture was loaded before
            bool alreadyLoaded = false;
            for (unsigned int j{0u}; j < loadedTextures.size(); j++) {
                if (std::strcmp(loadedTextures[j].path.data(), path.C_Str()) == 0) {
                    textures.push_back(loadedTextures[j]);
                    alreadyLoaded = true;
                    break;
                }
            }
            if (!alreadyLoaded) {
                Texture texture{
                	loadTextureFromFile(path.C_Str(), directory),
                	typeName,
                	path.C_Str()
                };
                textures.push_back(texture);
                loadedTextures.push_back(texture);
            }
		} else {
			std::cout << "Error: Cannot load a texture!" << std::endl;
		}
	}
	return textures;
}

unsigned int loadTextureFromFile(std::string file, std::string directory) {
	int imageWidth, imageHeight, colorChannels;
	unsigned char* data;
	unsigned int textureId{0};

	data = stbi_load((directory + '/' + file).c_str(), &imageWidth, &imageHeight, &colorChannels, 0);
	if (data != NULL) {
		GLenum format{GL_RGB};
        if (colorChannels == 1) {
            format = GL_RED;
        } else if (colorChannels == 3) {
            format = GL_RGB;
        } else if (colorChannels == 4) {
            format = GL_RGBA;
        }

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	} else {
		std::cout << "Cannot load a texture data!" << std::endl;
		stbi_image_free(data);
	}
	return textureId;
}
