#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
//uniform vec3 viewPos;
uniform float ambientLight;

void main()
{
	//float ambientLight = 0.3;
	vec3 ambient = ambientLight * lightColor;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize (lightPos-FragPos);
	float diffuseLight = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diffuseLight * lightColor;

	//int shininess = 128;
	//vec3 viewDir = normalize (viewPos - FragPos);
	//vec3 reflectDir = reflect (-lightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	//vec3 specular = spec* lightColor;
	
	vec3 result = (ambient + diffuse) * objectColor;
	//vec3 result = (ambient + diffuse + specular) * objectColor;

	FragColor = vec4 (result,1.0);
}