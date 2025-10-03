#version 440 core

// General parameters
#include "../ubo/rendering.glsl"
#include "../ubo/camera.glsl"
#include "../ubo/lights.glsl"

in vec3 SphereColor;        // Particle color from geometry shader
in vec2 FragPosition;     // Position within the quad (normalized)

out vec4 FragColor;       // Final output color

void main() 
{
	if(length(FragPosition - vec2(0.5)) > 0.5) discard;
	FragColor = vec4(FragPosition, 1, 1);
}