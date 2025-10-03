#version 440 core

#include "../ubo/camera.glsl"

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 PointColor;

uniform mat4 model;

void main()
{
    gl_Position = mCamera.MatV * model * vec4(aPos, 0.0, 1.0);
	PointColor = aColor;
}
