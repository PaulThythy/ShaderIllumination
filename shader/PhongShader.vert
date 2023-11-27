#version 450


uniform mat4 MVP;

layout(location = 0) in vec3 position; // the location specifies the stream/channel from which the data is retrieved (must match the location in the opengl code)


void main(){

	gl_Position = vec4(position, 1.);
	

}


