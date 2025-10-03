#version 440 core

out vec4 FragColor;

uniform vec3 lightColor;
uniform float intensity;

void main()
{
    FragColor = vec4(intensity * lightColor, 1.0);
} 