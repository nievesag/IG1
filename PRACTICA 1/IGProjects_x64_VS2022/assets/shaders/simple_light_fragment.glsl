#version 330 core

// Light components
const vec4 ambient = vec4(0, 0, 0, 1);
const vec4 diffuse = vec4(.5, .5, .5, 1);
const vec4 specular = vec4(0, 1, 0, 1);

// Exponent of the specular component
const int specularExponent = 8;

uniform vec4 lightDir; // light direction
uniform vec3 viewPos;  // viewer position (camera)
uniform vec4 color;  // uniform color for all vertices

in vec3 FragPos;    // fragment position in world coordinates
in vec3 Normal;     // normal vector
out vec4 FragColor;  // fixed-name color variable used by OpenGL

void main()
{
	// Diffuse component
	float diff_factor = max(dot(normalize(Normal), normalize(lightDir.rgb)), 0.0);

	// Specular component
	vec3 viewDir = normalize(viewPos - FragPos); // view direction (from the fragment to the camera)
	vec3 reflectDir = reflect(-lightDir.xyz, Normal);   // reflected light direction

	float spec_factor = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);

	// All together
	FragColor = color * (ambient + diff_factor * diffuse + spec_factor * specular);
}
