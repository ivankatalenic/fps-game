#version 330 core

// Shader inputs
in vec3 normalOut;
in vec2 texCoord;

// Uniform variables
uniform sampler2D texture_diffuse1;
// uniform material {
// 	sampler2D texture_diffuse1;
	// sampler2D texture_diffuse2;
	// sampler2D texture_diffuse3;
	// sampler2D texture_specular1;
	// sampler2D texture_specular2;
// }

// uniform float intensity;

// Shader outputs
out vec4 fragColor;

void main() {
	// fragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), intensity) * vec4(colorOut, 1.0f);
	// fragColor = vec4(colorOut, 1.0f);
	// fragColor = texture(material.texture_diffuse1, texCoord)
	// 		  * texture(material.texture_diffuse2, texCoord)
	// 		  * texture(material.texture_diffuse3, texCoord)
	// 		  * texture(material.texture_specular1, texCoord)
	// 		  * texture(material.texture_specular2, texCoord)
	// 		  * colorOut;
	fragColor = texture(texture_diffuse1, texCoord);
}
