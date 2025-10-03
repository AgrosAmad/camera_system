#version 440 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform vec3 Color;

out vec4 FragColor;

void main()
{
	FragColor = vec4(Color, 1.0);
}