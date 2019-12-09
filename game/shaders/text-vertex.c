#version 330 core

// (pos_x, pos_y, tex_s, tex_t)
layout (location = 0) in vec4 vertex;

uniform mat3 pos_matrix;
uniform mat3 tex_matrix;

out vec2 tex_coords;

void main() {
	gl_Position = vec4((pos_matrix * vec3(vertex.xy, 1.0f)).xy, 0.0f, 1.0f);
	tex_coords = (tex_matrix * vec3(vertex.zw, 1.0f)).xy;
}
