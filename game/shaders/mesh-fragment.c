#version 330 core

struct Material {
	vec3 color_ambient;
	vec3 color_diffuse;
	vec3 color_specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 color_ambient;
	vec3 color_diffuse;
	vec3 color_specular;
};

// Shader inputs
// Already in the view space
in vec3 normal_out;
in vec3 frag_position;
in vec2 tex_coord_out;

// Uniform variables
// uniform sampler2D texture_diffuse1;
// uniform material {
// 	sampler2D texture_diffuse1;
	// sampler2D texture_diffuse2;
	// sampler2D texture_diffuse3;
	// sampler2D texture_specular1;
	// sampler2D texture_specular2;
// }

// uniform float intensity;

uniform Material material;
uniform Light light;

uniform mat4 frag_mat_model;
uniform mat4 frag_mat_view;

// Shader outputs
out vec4 frag_color;

void main() {
	// frag_color = mix(texture(texture1, tex_coord_out), texture(texture2, tex_coord_out), intensity) * vec4(color_out, 1.0f);
	// frag_color = vec4(color_out, 1.0f);
	// frag_color = texture(material.texture_diffuse1, tex_coord_out)
	// 		  * texture(material.texture_diffuse2, tex_coord_out)
	// 		  * texture(material.texture_diffuse3, tex_coord_out)
	// 		  * texture(material.texture_specular1, tex_coord_out)
	// 		  * texture(material.texture_specular2, tex_coord_out)
	// 		  * color_out;
	// frag_color = texture(texture_diffuse1, tex_coord_out);

	// Ambient lighting component
	vec3 ambient = light.color_ambient * material.color_diffuse;

	vec3 light_position_in_view = vec3(frag_mat_view * frag_mat_model * vec4(light.position, 1.0f));
	vec3 normal = normalize(normal_out);
	vec3 light_direction = light_position_in_view - frag_position;
	float distance = length(light_direction);
	light_direction = normalize(light_direction);

	// Diffuse lighting component
	float diffuse_coefficient = max(dot(normal, light_direction), 0.0f);
	vec3 diffuse = light.color_diffuse * diffuse_coefficient * material.color_diffuse;

	vec3 view_direction = normalize(-1.0f * frag_position);
	vec3 reflection_direction = reflect(-light_direction, normal);

	// Specular lighting component
	float specular_coefficient = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = light.color_specular * specular_coefficient * material.color_specular;

	// const float light_power = 20.0f;
	// const float division_constant = 0.5f;
	// vec3 result_color = ambient + light_power * (diffuse + specular) / (distance + division_constant);
	vec3 result_color = ambient + diffuse + specular;

	frag_color = vec4(result_color, 1.0f);
}
