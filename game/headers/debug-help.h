#ifndef DEBUG_HELP_H
#define DEBUG_HELP_H

#include <iostream>
#include <iomanip>
#include <string>

#include <glm/glm.hpp>

namespace debug {

	inline void print_vec(glm::vec3 vec, const std::string& name) {
		std::cout << name 
			<< ": ("
			<< std::fixed << std::setprecision(6)
			<< vec.x << ", "
			<< vec.y << ", "
			<< vec.z << ")"
			<< std::endl;
	}

} // debug

#endif // DEBUG_HELP_H
