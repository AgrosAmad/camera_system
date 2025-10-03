#version 440 core

uniform vec3 BoxColor;

out vec4 FragColor;

void main() 
{
	FragColor = vec4(BoxColor, 1);
}