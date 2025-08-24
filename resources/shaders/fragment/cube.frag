#version 460 core

out vec4 FragColor;

//uniform sampler2D Texture0;	
//uniform sampler2D Texture1;

uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{
    FragColor = vec4(lightColor * objectColor, 1.0);
	//FragColor = mix(texture(Texture0, TexCoords), texture(Texture1, TexCoords), 0.2);
}