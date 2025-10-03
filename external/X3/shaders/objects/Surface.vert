#version 440 core

#include "../ubo/camera.glsl"

layout (location = 0) in vec3 aPos;

uniform mat4 model;

out vec3 Position;

void main()
{
    gl_Position = mCamera.MatP * mCamera.MatV * model * vec4(aPos, 1.0);
	Position = aPos;
}
