#define MAX_NUM_LIGHTS 20

struct Light
{			  
	// Spatial params
	vec3 position;
	vec3 direction;
	
	// Color
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float intensity;
	
	// Misc
	vec3 attenuation;
	vec2 coneAngle;
};

layout (std140, binding = 1) uniform ubo_lights_point
{	
	Light mLightPoint[MAX_NUM_LIGHTS];
};

layout (std140, binding = 2) uniform ubo_lights_direc
{	
	Light mLightDirec[MAX_NUM_LIGHTS];
};

layout (std140, binding = 3) uniform ubo_lights_spot
{	
	Light mLightSpot[MAX_NUM_LIGHTS];
};