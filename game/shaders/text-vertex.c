#version 330 core

// vec4 = (pos_x, pos_y, tex_s, tex_t)
layout (location = 0) in vec4 vertex;
out vec2 tex_coords;

void main() {
	gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
	tex_coords = vertex.zw;
}
