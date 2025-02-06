#version 330 core

// Object material
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};

uniform Material material;  // object material
uniform vec3 viewPos;  // viewer position (camera)

// Directional light
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	bool enabled;
};

// Positional light
struct PosLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	bool enabled;
};

// Spotlight
struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
	float exponent;

	bool enabled;
};

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient  = light.ambient * material.ambient;
	vec3 diffuse  = light.diffuse * diff * material.diffuse;
	vec3 specular = light.specular * spec * material.specular;

	return ambient + diffuse + specular;
}

vec3 calcPosLight(PosLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    return attenuation * (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    return attenuation * intensity * (ambient + diffuse + specular);
}

const int NR_DIR_LIGHTS = 2;
const int NR_POS_LIGHTS = 4;
const int NR_SPOT_LIGHTS = 4;

uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform PosLight posLights[NR_POS_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

in vec3 FragPos;    // fragment position in world coordinates
in vec3 Normal;     // normal vector
out vec4 FragColor;  // fixed-name color variable used by OpenGL

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0);

	// phase 1: Directional lighting
	for (int i = 0; i < NR_DIR_LIGHTS; i++)
		if (dirLights[i].enabled)
			result += calcDirLight(dirLights[i], norm, viewDir);

	// phase 2: Point lights
	for (int i = 0; i < NR_POS_LIGHTS; i++)
		if (posLights[i].enabled)
			result += calcPosLight(posLights[i], norm, FragPos, viewDir);

	// phase 3: Spot light
	for (int i = 0; i < NR_SPOT_LIGHTS; i++)
		if (spotLights[i].enabled)
			result += calcSpotLight(spotLights[i], norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}
