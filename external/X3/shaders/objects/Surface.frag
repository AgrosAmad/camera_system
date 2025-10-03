#version 440 core

uniform vec3 SurfColor;

out vec4 FragColor;

void main() 
{
	FragColor = vec4(SurfColor, 1.f);
}
