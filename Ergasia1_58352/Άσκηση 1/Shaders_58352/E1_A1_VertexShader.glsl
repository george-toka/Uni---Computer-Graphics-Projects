#version 330 core 

layout (location = 0) in vec3 inputPosition;

void main() 
{ 
	gl_Position = vec4(inputPosition.x, inputPosition.y, inputPosition.z, 1.0);
}


