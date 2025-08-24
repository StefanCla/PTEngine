#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
   gl_Position = Projection * View * vec4(aPos, 1.0f);
   FragPos = vec3(Model * vec4(aPos, 1.0f));

   //Avoid inverse in shaders as they are costly. In thise case a normal matrix is probably preffered
   Normal = mat3(transpose(inverse(Model))) * aNormal;
   TexCoords = aTexCoords;
};