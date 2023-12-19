#version 450


uniform mat4 MVP;
uniform mat4 MODEL;

layout(location = 0) in vec3 position; // the location specifies the stream/channel from which the data is retrieved (must match the location in the opengl code)
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 normal;

uniform struct Light {
	vec3 position;
	vec3 intensities;
	float ambientCoefficient;
	float attenuation;
} light;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 TexCoord;

void main(){
	//gl_Position = vec4(position, 1.);
	
	gl_Position = MVP * vec4(position, 1.);
	fragPosition = vec3(MODEL * vec4(position, 1.0));
	fragNormal = normal;
	TexCoord = vertexUV;
}


