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
in vec3 fragNormal;

out vec4 finalColor;

void main() {

	// color is the x,y,z fragment position
	//finalColor = vec4(fragPosition, 1.);

	// Calculate ambient light
    //vec3 ambientLight = light.intensities * light.ambientCoefficient;

	// Output final color (without texture)
    //finalColor = vec4(ambientLight, 1.0);

	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(light.position - fragPosition);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.intensities * diff;

	finalColor = vec4(diffuse, 1.0);
}
