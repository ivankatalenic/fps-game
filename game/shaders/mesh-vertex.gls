#version 330 core

// Shader inputs
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

// Uniform variables
uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;

// Shader outputs
out vec3 normal_out;
out vec3 frag_position;
out vec2 tex_coord_out;

void main() {
	gl_Position = mat_projection * mat_view * mat_model * vec4(position, 1.0f);
	normal_out = mat3(transpose(inverse(mat_view * mat_model))) * normal;
	frag_position = vec3(mat_view * mat_model * vec4(position, 1.0f));
	tex_coord_out = tex_coord;
}
