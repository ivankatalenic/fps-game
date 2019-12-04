#version 330 core

in vec2 tex_coords;

uniform sampler2D tex;
uniform vec3 color;

out vec4 color_out;

void main() {
	color_out = vec4(1.0f, 1.0f, 1.0f, texture(tex, tex_coords).r) * vec4(color, 1.0f);
}
