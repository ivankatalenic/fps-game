#ifndef DEBUG_HELP_HH
#define DEBUG_HELP_HH

#include "external/glm/glm/glm.hpp"

#include <iostream>
#include <iomanip>
#include <string>

namespace debug {

	inline void print_str(const std::string& str) {
		std::cout << str << std::endl;
	}

	inline void print_vec(glm::vec3 vec, const std::string& name) {
		std::cout << name 
			<< ": ("
			<< std::fixed << std::setprecision(6)
			<< vec.x << ", "
			<< vec.y << ", "
			<< vec.z << ")"
			<< std::endl;
	}

	template <typename T>
	inline void print_var(T var, const std::string& name) {
		std::cout << name << ": "
			<< var << std::endl;
	}

	template<>
	inline void print_var(float var, const std::string& name) {
		std::cout << name << ": "
			<< std::fixed << std::setprecision(6)
			<< var << std::endl;
	}

	template<>
	inline void print_var(double var, const std::string& name) {
		std::cout << name << ": "
			<< std::fixed << std::setprecision(6)
			<< var << std::endl;
	}

} // debug

#endif // DEBUG_HELP_HH
