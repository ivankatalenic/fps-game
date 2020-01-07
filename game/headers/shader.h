#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <glm/glm.hpp>

class Shader {
public:
	Shader() = default;
	/**
	 * Takes paths to shaders' source files.
	 */
	Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

	unsigned int id;

	// Sets the program for use
	void use() const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setMat3(const std::string& name, glm::mat3 value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
private:
};

#endif
