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

	//distance between surface and light source
	float distance = length(light.position - fragPosition);

	//attenuation calculation
	float attenuation = 1.0 / (1.0 + light.attenuation * distance + light.attenuation * distance * distance);

	//diffuse light
	vec3 normal = normalize(fragNormal);
	vec3 lightDir = normalize(light.position - fragPosition);

	float diff = max(dot(normal, lightDir), 0.0);
	// diffuse only for diffuse lighting
	//vec3 diffuse = light.intensities * diff;
	// diffuse for specular lighting with attenuation
	vec3 diffuse = light.intensities * diff * attenuation;

	//finalColor = vec4(diffuse, 1.0);

	//specular light
	vec3 viewDir = normalize(cameraPosition - fragPosition);

	vec3 reflectDir = reflect(-lightDir, normal);
	
	//specular component calculation
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), materialShininess);
	//specular only for specular lighting
	//vec3 specular = light.intensities * materialSpecularColor * spec;
	//specular for specular lightinh with attenuation
	vec3 specular = light.intensities * materialSpecularColor * spec * attenuation;

	finalColor = vec4(diffuse + specular, 1.0);
}
