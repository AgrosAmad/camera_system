#version 440 core

#include "../ubo/camera.glsl"

layout (location = 0) in float aPos;

uniform mat4 model;

void main()
{
    gl_Position = mCamera.MatP * mCamera.MatV * model * vec4(aPos, 0.0, 0.0, 1.0);
}
