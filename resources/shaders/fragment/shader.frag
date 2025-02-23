#version 460 core

out vec4 FragColor;

in vec4 ourColor;
in vec2 TexCoords;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
	FragColor = mix(texture(Texture0, TexCoords), texture(Texture1, TexCoords), 0.2);
}