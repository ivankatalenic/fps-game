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

#include <debug-help.h>

struct collision_result {
	float relative_distance;
	glm::vec3 point;
	glm::vec3 normal;
};

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
bool Model::checkCollision(
		glm::vec3 position,
		glm::vec3 step,
		glm::vec3& return_step,
		const Polygon* last_collision_polygon
	) {

	constexpr float radius{1.0f};

	bool is_collision_detected{false};

	collision_result main_collision{};
	main_collision.relative_distance = FLT_MAX;

	Polygon* main_collision_polygon{nullptr};

	for (Mesh& mesh : meshes) {
		for (Polygon& polygon : mesh.polygons) {
			
			bool is_triangle_collision{false};
			collision_result triangle_collision;
			triangle_collision.normal = polygon.normal;

			// Adjust the triangle plane's normal to face the player
			float plane_dist{glm::dot(polygon.normal, position) + polygon.d};
			if (plane_dist < 0.0f) {
				triangle_collision.normal = -polygon.normal;
				plane_dist = -plane_dist;
			}

			// Check if the sphere is inside the triangle's bounds
			constexpr float radius_tolerance{1e-2};
			if (plane_dist < radius - radius_tolerance) {
				// Check if the projection is inside triangle
				const glm::vec3 projection(position - plane_dist * triangle_collision.normal);
				if (math_aux::is_point_inside_triangle(projection,
						polygon.vertices[0].position,
						polygon.vertices[1].position,
						polygon.vertices[2].position)) {
					// debug::print_str("Inside a triangle's plane");

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
					// debug::print_str("Inside a triangle's edge");
					continue;
				}
			}


			// Check if the sphere will hit triangle's parallel bound planes
			const float triangle_normal_dot_step{glm::dot(triangle_collision.normal, step)};
			// Check if the step vector faces the plane
			if (triangle_normal_dot_step < 0.0f) {
				// The step vector is not parallel with the plane
				// Relative distance to the first parallel plane
				triangle_collision.relative_distance = 
					(radius - polygon.d - glm::dot(triangle_collision.normal, position))
						/ triangle_normal_dot_step;
				if (triangle_collision.relative_distance >= 0.0f
						&& triangle_collision.relative_distance < 1.0f) {
					triangle_collision.point =
						position + triangle_collision.relative_distance * step;
					const glm::vec3 projection(
						triangle_collision.point - radius * triangle_collision.normal
					);
					if (math_aux::is_point_inside_triangle(projection,
							polygon.vertices[0].position,
							polygon.vertices[1].position,
							polygon.vertices[2].position)) {
						is_triangle_collision = true;
						// debug::print_str("Hitting triangle's parallel plane");
					}
				}
			}


			// Check if the sphere will hit triangle's edges
			if (!is_triangle_collision) {
				bool is_edge_collision{false};
				collision_result edge_collision;
				edge_collision.relative_distance = FLT_MAX;

				// Check every edge of the triangle
				for (int i{0}; i < 3; i++) {
					const glm::vec3 p_start(polygon.vertices[i].position);
					const glm::vec3 p_end(polygon.vertices[(i + 1) % 3].position);
					if (auto status{
							math_aux::is_colliding_with_cylinder(position, step,
								p_start, p_end, radius)
						}) {
						const float relative_distance{
							glm::length(*status - position) / glm::length(step)
						};
						const glm::vec3 axis(glm::normalize(p_end - p_start));
						const glm::vec3 normal(
							(*status - p_start)
								- glm::dot(*status - p_start, axis) * axis
						);
						const float normal_dot_step{glm::dot(normal, step)};
						if (relative_distance >= 0.0f && relative_distance < 1.0f
								&& !math_aux::is_zero(normal_dot_step)
								&& normal_dot_step < 0.0f
								&& relative_distance < edge_collision.relative_distance) {
							is_edge_collision = true;
							edge_collision.relative_distance = relative_distance;
							edge_collision.point = *status;
							edge_collision.normal = normal;
							// debug::print_var(normal_dot_step, "normal_dot_step");
						}
					}
				} // for (edge)

				// Found an edge which collides with the step vector
				if (is_edge_collision) {
					triangle_collision = edge_collision;
					is_triangle_collision = true;

					// debug::print_str("Hitting a triangle's edge");
					// debug::print_var(triangle_collision.relative_distance, "triangle_collision.relative_distance");
					// debug::print_var(glm::dot(triangle_collision.normal, step), "triangle_normal_dot_step");
				}
			} // Triangle's edges collision check
			

			// Check if the sphere will hit triangle's vertices
			if (!is_triangle_collision) {
				bool is_vertex_collision{false};
				collision_result vertex_collision;
				vertex_collision.relative_distance = FLT_MAX;

				for (int i{0}; i < 3; i++) {
					const glm::vec3 vertex(polygon.vertices[i].position);
					if (auto status{
							math_aux::is_colliding_with_sphere(position, step,
								vertex, radius)
						}) {
						const float relative_distance{
							glm::length(*status - position) / glm::length(step)
						};
						const glm::vec3 normal(*status - vertex);
						const float normal_dot_step{glm::dot(normal, step)};
						if (relative_distance >= 0.0f && relative_distance < 1.0f
								&& !math_aux::is_zero(normal_dot_step)
								&& normal_dot_step < 0.0f
								&& relative_distance < vertex_collision.relative_distance) {
							vertex_collision.relative_distance = relative_distance;
							vertex_collision.normal = normal;
							vertex_collision.point = *status;
							is_vertex_collision = true;
						}
					}
				} // for (vertex)

				// Found a vertex which collides with the step vector
				if (is_vertex_collision) {
					triangle_collision = vertex_collision;
					is_triangle_collision = true;
					// debug::print_str("Hitting triangle's vertex");
				}
			} // Triangle's vertices collision check

			if (is_triangle_collision
					&& triangle_collision.relative_distance
						< main_collision.relative_distance) {
				main_collision = triangle_collision;
				is_collision_detected = true;			
				main_collision_polygon = &polygon;
			}

		} // for (polygon)
	} // for (mesh)

	if (is_collision_detected
			&& main_collision_polygon != last_collision_polygon) {
		main_collision.normal = glm::normalize(main_collision.normal);
		// The vector from the position to the point of collision.
		// The vector before the collision point.
		const glm::vec3 step_before(main_collision.point - position);
		// The vector from the point of collision to the end of the step vector
		// The vector after the collision point.
		// Also know as the penetration vector.
		const glm::vec3 step_after(step - step_before);
		const float collision_normal_dot_step_after{glm::dot(main_collision.normal, step_after)};
		// The vector that is perpendicular to the surface of the point of collision.
		// The tangent vector to the collision surface.
		// Also know as the resolution vector.
		const glm::vec3 step_tangent(step_after - collision_normal_dot_step_after * main_collision.normal);


		// Don't do the new collision check if the resolution vector
		// can't collide with any other object.
		// The step_before vector leads to the first object
		// which collides with the step vector.
		// The step_tangent vector, which is calculated directly using
		// the step_after vector, could collide with some other
		// object, but only if its length is non zero.
		//
		// And
		//
		// Don't do the new collision check if the resolution vector
		// can't be accurately calculated because of the limited
		// floating point data type precision.
		if (!math_aux::is_zero(glm::length(step_after))) {
			// Check if the new step vector doesn't collide with other objects
			// The vector which, ideally, would resolve the collision.
			const glm::vec3 new_step(step_before + step_tangent);
			glm::vec3 recursion_return_step;
			bool is_new_collision_detected{
				checkCollision(
					position,
					new_step,
					recursion_return_step,
					main_collision_polygon
				)
			};
			if (is_new_collision_detected) {
				return_step = recursion_return_step;
			} else {
				return_step = new_step;
			}
		} else {
			// The step_after vector, aka the penetration vector,
			// is zero, hence use only a vector up to the point of collision,
			// aka the step_before vector.
			// debug::print_vec(step_before, "step_before");
			// debug::print_vec(step_after, "step_after");
			// debug::print_vec(step_tangent, "step_tangent");
			// debug::print_vec(main_collision.normal, "normal");

			// debug::print_var(main_collision.relative_distance, "relative_distance");
			// debug::print_var(glm::dot(main_collision.normal, step), "normal_dot_step");
			// debug::print_var(collision_normal_dot_step_after, "collision_normal_dot_step_after");

			return_step = step_before;
		}

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
