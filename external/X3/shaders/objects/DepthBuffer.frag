#version 440 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 FragColor;

uniform float zNear, zFar;

float LinearizeDepth(float depth);

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
	FragColor = vec4(vec3(depth), 1.0);
}

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}