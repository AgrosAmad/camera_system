struct Rendering
{			  
	int NumLightDirec;
	int NumLightPoint;
	int NumLightSpot;
	float Time;
};

layout (std140, binding = 5) uniform ubo_rendering
{	
	Rendering mRender;
};