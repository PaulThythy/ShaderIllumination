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

	// color is the x,y,z fragment position
	//finalColor = vec4(fragPosition, 1.);

	// Calculate ambient light
    vec3 ambientLight = light.intensities * light.ambientCoefficient;

	// Output final color (without texture)
    finalColor = vec4(ambientLight, 1.0);
}
