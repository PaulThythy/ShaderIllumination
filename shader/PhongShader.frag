#version 450

uniform vec3 cameraPosition;

uniform sampler2D myTextureSampler;
uniform mat4 MODEL;
uniform float materialShininess;
uniform vec3 materialSpecularColor;

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

	//diffuse light
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(light.position - fragPosition);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.intensities * diff;

	//finalColor = vec4(diffuse, 1.0);

	//specular light
	vec3 viewDir = normalize(cameraPosition - fragPosition);

	vec3 reflectDir = reflect(-lightDir, normal);
	
	//specular component calculation
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), materialShininess);
	vec3 specular = light.intensities * materialSpecularColor * spec;

	finalColor = vec4(diffuse + specular, 1.0);
}
