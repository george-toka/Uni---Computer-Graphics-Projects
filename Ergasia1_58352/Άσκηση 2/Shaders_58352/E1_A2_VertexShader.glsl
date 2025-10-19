#version 330 core 

layout (location = 0) in vec3 inputPosition;

uniform vec2 displacement;

out vec3 color;

float fPointX = inputPosition.x + displacement.x;
float fPointY = inputPosition.y + displacement.y;

void main() 
{ 
	gl_Position = vec4(fPointX, fPointY, 0.0f, 1.0);
	//color value normalisation formula
	color = vec3(0.5 *(fPointX + 1), 0.5 * (fPointY + 1), 0.0f);  
}


