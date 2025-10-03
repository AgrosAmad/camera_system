#version 440 core

uniform vec3 MeshColor;

out vec4 FragColor;

void main() 
{
	FragColor = vec4(MeshColor, 1);
}
