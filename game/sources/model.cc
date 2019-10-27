#include <model.h>

#include <assimp/Importer.hpp>
#include <assimp/vector3.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <cmath>
#include <cfloat>
#include <optional>

#include <math-aux.h>

#include <iostream>
#include <iomanip>

unsigned int loadTextureFromFile(std::string file, std::string directory);

Model::Model(const char* path) {
	loadModel(std::string(path));
}

void Model::draw(const Shader& shader) {
	for (auto& mesh : meshes) {
		mesh.draw(shader);
	}
}

/**
 * Returns a modified velocity vector if a collision
 * is detected.
 */
bool Model::checkCollision(glm::vec3 position, glm::vec3 step, glm::vec3& return_step) {
	constexpr float radius{1.0f};
	constexpr float error_tolerance{1e-5};

	bool is_collision_detected{false};

	float     collision_lambda{FLT_MAX};
	glm::vec3 collision_point;
	glm::vec3 collision_normal;

	for (Mesh& mesh : meshes) {
		for (Polygon& polygon : mesh.polygons) {
			
			bool      triangle_collision{false};
			float     triangle_lambda;
			glm::vec3 triangle_intersection;
			glm::vec3 triangle_normal(polygon.normal);

			// Adjust the triangle plane's normal to face the player
			float plane_dist{glm::dot(polygon.normal, position) + polygon.d};
			if (plane_dist < 0.0f) {
				triangle_normal = -polygon.normal;
				plane_dist = -plane_dist;
			}

			// Check if the sphere is inside the triangle's bounds
			constexpr float radius_tolerance{1e-2};
			if (plane_dist < radius - radius_tolerance) {
				// Check if the projection is inside triangle
				const glm::vec3 projection(position - plane_dist * triangle_normal);
				if (math_aux::is_point_inside_triangle(projection,
						polygon.vertices[0].position,
						polygon.vertices[1].position,
						polygon.vertices[2].position)) {
					// std::cout << "Inside triangle's plane" << std::endl;
					// Skip the triangle
					continue;
				}

				// Distance to the triangle's edges, and vertices
				bool skip_triangle{false};
				for (int i{0}; i < 3; i++) {
					glm::vec3 p_start(polygon.vertices[i].position);
					glm::vec3 p_end(polygon.vertices[(i + 1) % 3].position);
					if (math_aux::distance_to_line(position, p_start, p_end)
							< radius - radius_tolerance) {
						// Skip the triangle
						skip_triangle = true;
						break;
					}
				}
				if (skip_triangle) {
					// std::cout << "Inside triangle's edge" << std::endl;
					continue;
				}
			}


			// Check if the sphere will hit triangle's parallel bound planes
			const float triangle_normal_dot_step{glm::dot(triangle_normal, step)};
			if (triangle_normal_dot_step < 0.0f) {
				// The step vector is not parallel with the plane
				// Relative distance to the first parallel plane
				triangle_lambda = (radius - polygon.d - glm::dot(triangle_normal, position)) 
					/ triangle_normal_dot_step;
				if (triangle_lambda >= 0.0f && triangle_lambda < 1.0f) {
					// The step vector intersects the first parallel plane
					// std::cout << "step = " 
					// 	<< std::fixed 
					// 	<< std::setprecision(6) << glm::length(step) << std::endl;
					// std::cout << "triangle_lambda = " << triangle_lambda << std::endl;
					triangle_intersection = position + triangle_lambda * step;
					const glm::vec3 projection(
						triangle_intersection - radius * triangle_normal
					);
					if (math_aux::is_point_inside_triangle(projection,
							polygon.vertices[0].position,
							polygon.vertices[1].position,
							polygon.vertices[2].position)) {
						triangle_collision = true;
						// std::cout << "Hitting triangle's parallel plane" << std::endl;
					}
				} else {
					// Cannot reach triangle's parallel bound plane
					// Skip the triangle
					continue;
				}
			}


			// Check if the sphere will hit triangle's edges
			if (!triangle_collision) {
				bool found_edge{false};
				float edge_lambda{FLT_MAX};
				glm::vec3 edge_intersection;
				glm::vec3 edge_p_start;
				glm::vec3 edge_p_end;

				// Check every edge of the triangle
				for (int i{0}; i < 3; i++) {
					const glm::vec3 p_start(polygon.vertices[i].position);
					const glm::vec3 p_end(polygon.vertices[(i + 1) % 3].position);
					if (auto status{
							math_aux::is_colliding_with_cylinder(position, step,
								p_start, p_end, radius)
						}) {
						const float lambda{
							glm::length(*status - position) / glm::length(step)
						};
						if (lambda > 0.0f && lambda < 1.0f
								&& lambda < edge_lambda) {
							found_edge = true;
							edge_lambda = lambda;
							edge_intersection = *status;
							edge_p_start = p_start;
							edge_p_end = p_end;
						}
					}
				} // for (edge)

				// Found an edge which collides with the step vector
				if (found_edge) {
					const glm::vec3 axis(glm::normalize(edge_p_end - edge_p_start));
					triangle_normal = (edge_intersection - edge_p_start)
						- glm::dot(edge_intersection - edge_p_start, axis) * axis;
					triangle_intersection = edge_intersection;
					triangle_lambda = edge_lambda;
					triangle_collision = true;
					std::cout << "Hitting triangle's edge" << std::endl;
				}
			} // Triangle's edges collision check
			

			// Check if the sphere will hit triangle's vertices
			if (!triangle_collision) {
				bool found_vertex{false};
				float vertex_lambda{FLT_MAX};
				glm::vec3 vertex_intersection;
				glm::vec3 vertex_normal;
				for (int i{0}; i < 3; i++) {
					const glm::vec3 vertex(polygon.vertices[i].position);
					if (auto status{
							math_aux::is_colliding_with_sphere(position, step,
								vertex, radius)
						}) {
						const float lambda{
							glm::length(*status - position) / glm::length(step)
						};
						if (lambda > 0.0f && lambda < 1.0f
								&& lambda < vertex_lambda) {
							vertex_lambda = lambda;
							vertex_normal = *status - vertex;
							vertex_intersection = *status;
							found_vertex = true;
						}
					}
				} // for (vertex)

				// Found a vertex which collides with the step vector
				if (found_vertex) {
					triangle_lambda = vertex_lambda;
					triangle_normal = vertex_normal;
					triangle_intersection = vertex_intersection;
					triangle_collision = true;
					std::cout << "Hitting triangle's vertex" << std::endl;
				}
			} // Triangle's vertices collision check

			if (triangle_collision && triangle_lambda < collision_lambda) {
				collision_lambda = triangle_lambda;
				collision_point = triangle_intersection;
				collision_normal = triangle_normal;
				is_collision_detected = true;
			}

		} // for (polygon)
	} // for (mesh)

	if (is_collision_detected) {
		collision_normal = glm::normalize(collision_normal);
		glm::vec3 step_before(collision_point - position);
		glm::vec3 step_after(step - step_before);
		glm::vec3 step_tangent(step_after - glm::dot(collision_normal, step_after) * collision_normal);
		glm::vec3 new_step(step_before + step_tangent);

		glm::vec3 recursion_step;
		// Check if the new step doesn't collide
		bool collision_status{checkCollision(position, new_step, recursion_step)};
		if (collision_status) {
			return_step = recursion_step;
		} else {
			return_step = new_step;
		}
		// std::cout << "Collision detected!" << std::endl;
		return true;
	}
	return false;
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
			meshes.push_back(processMesh(theMesh, scene));
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

		vertices.push_back({position, normal, texCoords});
	}

	// Setting up indices
	std::vector<unsigned int> indices;
	std::vector<Polygon> polygons;
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
