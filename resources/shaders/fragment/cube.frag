#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
//uniform sampler2D Texture0;	
//uniform sampler2D Texture1;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float AmbientStrenght = 0.1;
	vec3 Ambient = AmbientStrenght * lightColor;

	float SpecularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	int shininess = 32;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 Specular = SpecularStrength * spec * lightColor;

	vec3 Result = (Ambient + diffuse + Specular) * objectColor;
    FragColor = vec4(Result, 1.0);
	//FragColor = mix(texture(Texture0, TexCoords), texture(Texture1, TexCoords), 0.2);
}