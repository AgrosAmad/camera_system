struct Camera
{
    mat4 MatP;    // 64 bytes
    mat4 MatV;    // 64 bytes
    vec3 Pos;     // 12 bytes
    float padding1; // 4 bytes (padding for alignment)
    vec3 Dir;     // 12 bytes
    float zNear;  // 4 bytes
    float zFar;   // 4 bytes
    float padding2; // 4 bytes (padding for alignment)
};

layout (std140, binding = 0) uniform ubo_camera
{	
	Camera mCamera;
};