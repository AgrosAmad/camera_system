#version 440 core

uniform sampler2D ColorMap;
uniform float MinPot;
uniform float MaxPot;

in float Pot;

out vec4 FragColor;

void main() 
{
	float texCoord = clamp((Pot - MinPot) / (MaxPot - MinPot), 0.f, 1.f);
	FragColor = 2.f * texture(ColorMap, vec2(texCoord, 0.5));
}