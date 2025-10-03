struct Camera
{			  
	mat4 MatP; 
	mat4 MatV; 
	vec3 Pos; 
	vec3 Dir;  
	float zNear;   
	float zFar;    
};

layout (std140, binding = 0) uniform ubo_camera
{	
	Camera mCamera;
};