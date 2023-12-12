#version 450

uniform vec3 cameraPosition;

uniform sampler2D myTextureSampler;
uniform mat4 MODEL;

// same as vertex shader because uniform memory is common to all shaders
uniform struct Light {
	vec3 position;
	vec3 intensities;
	float ambientCoefficient;
	float attenuation;
} light;

in vec3 fragPosition;

out vec4 finalColor;

void main() {

	finalColor = vec4(fragPosition, 1.);

	

	//finalColor = vec4(1.0, 0.0, 0.0, 1.0);
	//finalColor = vec4(finalColor, 1.0);

}
