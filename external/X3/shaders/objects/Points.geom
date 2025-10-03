#version 440 core

#include "../ubo/camera.glsl"

layout(points) in;                            // Input: points
layout(triangle_strip, max_vertices = 5) out; // Output: quads

in vec3 PointColor[];  // Input particle color
out vec3 SphereColor;        // Pass color to fragment shader
out vec2 FragPosition;     // Position on the quad for sphere simulation

uniform float SphereRadius; // Radius of particles (in screen space)

void point(vec3 p, float s)
{
	vec3 lp0 = p + vec3(-1,1,0) * s;
	vec3 lp1 = p + vec3(-1,-1,0) * s;
	vec3 lp2 = p + vec3(1,1,0) * s;
	vec3 lp3 = p + vec3(1,-1,0) * s;
	
	FragPosition = vec2(0,1);
	gl_Position = mCamera.MatP * vec4(lp0, 1);
	EmitVertex();
	
	FragPosition = vec2(0,0);
	gl_Position = mCamera.MatP * vec4(lp1, 1);
	EmitVertex();
	
	FragPosition = vec2(1,1);
	gl_Position = mCamera.MatP * vec4(lp2, 1);
	EmitVertex();
	
	FragPosition = vec2(1,0);
	gl_Position = mCamera.MatP * vec4(lp3, 1);
	EmitVertex();
	
	EndPrimitive();
}

void main() 
{
	SphereColor = PointColor[0];
	point(gl_in[0].gl_Position.xyz, SphereRadius);
}
