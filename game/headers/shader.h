#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
	public:
		unsigned int id;

		// Shader constructor: takes in path to 
		// vertex and fragment shader source file
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

		// Empty constructor
		Shader();

		// Sets program for use
		void use();

		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVec3(const std::string& name, glm::vec3 value) const;
		void setMat4(const std::string& name, glm::mat4 value) const;
	private:
};

#endif
