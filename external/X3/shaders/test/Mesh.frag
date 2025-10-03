#version 440 core

#define MAX_NUM_LIGHTS 10

struct Material
{
	bool hasMaterial[3];
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
	vec3 color;
};

struct Light {

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

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 FragColor;

// Lights
uniform int numLightDirec, numLightPoint, numLightSpot;
uniform Light lightsDirec[MAX_NUM_LIGHTS];
uniform Light lightsPoint[MAX_NUM_LIGHTS];
uniform Light lightsSpot[MAX_NUM_LIGHTS];

// Mesh material
uniform Material material;

// Misc
uniform vec3 viewPos;
uniform float time;


vec3 PhongShading();
vec3 ShadingGeneralLight(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap, vec3 lightDir, float intensity, float attenuation);
vec3 ShadingDirec(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap);
vec3 ShadingPoint(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap);
vec3 ShadingSpot(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap);


void main()
{	
	
		
	vec3 result = PhongShading();
	FragColor = vec4(1.0);
}

vec3 PhongShading()
{

	// Common vectors
	vec4 diffMap = material.hasMaterial[0] ? texture(material.diffuse, TexCoord) : vec4(material.color, 1.f);
	vec3 specMap = material.hasMaterial[1] ? vec3(texture(material.specular, TexCoord)) : material.color;
	vec3 emisMap = material.hasMaterial[2] ? vec3(texture(material.emission, TexCoord)) : vec3(0.f);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 normal = normalize(Normal);
	
	// Discard transparent textures
	if( diffMap.a < 0.5f) discard;
	
	vec3 result = vec3(0.f);
	
	// Directional lights
	for(int i = 0; i < numLightDirec; i++)
	{
		result += ShadingDirec(lightsDirec[i], normal, viewDir, diffMap.xyz, specMap, emisMap);
	}

	// Point lights
	for(int i = 0; i < numLightPoint; i++)
	{
		result += ShadingPoint(lightsPoint[i], normal, viewDir, diffMap.xyz, specMap, emisMap);
	}	

	// Spot lights
	for(int i = 0; i < numLightSpot; i++)
	{
		result += ShadingSpot(lightsSpot[i], normal, viewDir, diffMap.xyz, specMap, emisMap);
	}	

	return result;
}

vec3 ShadingDirec(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap)
{
	
	// Directional light
	float intensity = light.intensity;
	vec3 lightDir = normalize(-light.direction);	
	float attenuation = 1.f;
	
	return ShadingGeneralLight(light, normal, viewDir, diffMap, specMap, emisMap, lightDir, intensity, attenuation);
}

vec3 ShadingPoint(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap)
{

	// Point light
	float intensity = light.intensity;
	vec3 lightDir = normalize(light.position - FragPos);
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));
	
	return ShadingGeneralLight(light, normal, viewDir, diffMap, specMap, emisMap, lightDir, intensity, attenuation);
}

vec3 ShadingSpot(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap)
{
	
	// light types
	float intensity = light.intensity;
	vec3 lightDir = vec3(0.f);
	float attenuation = 1.f;

	lightDir = normalize(light.position - FragPos);
	float distance = length(light.position - FragPos);
	attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));
	
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.coneAngle.x - light.coneAngle.y;
	intensity = intensity * clamp((theta - light.coneAngle.y) / epsilon, 0.f, 1.f);	
	
	// ambient
	vec3 ambient = light.ambient * diffMap;
	
	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * diffMap);
	
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * specMap);
	
	// emission
	vec3 emission = emisMap;
    
	
	// phong lighting
	return intensity * attenuation * (ambient + diffuse + specular) + emission;
}


vec3 ShadingGeneralLight(Light light, vec3 normal, vec3 viewDir, vec3 diffMap, vec3 specMap, vec3 emisMap, vec3 lightDir, float intensity, float attenuation)
{	
	// ambient
	vec3 ambient = light.ambient * diffMap;
	
	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * diffMap);
	
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * specMap);
	
	// emission
	vec3 emission = emisMap;
    	
	// phong lighting
	return intensity * attenuation * (ambient + diffuse + specular) + emission;
}
