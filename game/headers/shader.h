#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <glad/glad.h>

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

		void setBool(const std::string& name, bool value);
		void setInt(const std::string& name, int value);
		void setFloat(const std::string& name, float value);
	private:
};

#endif
