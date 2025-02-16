#pragma once
#include <stdint.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <gl/GL.h>

class Renderer2
{
public:
	Renderer2();
	~Renderer2();

	bool SetupWindow(int32_t Width, int32_t Height);
	void SwapWindow();
	void ResizeWindow(int32_t Width, int32_t Height);

	//Setup Vertex Buffer Object
	void SetupVBO();

	//Setup Vertex Shader
	void SetupVertexShader();

private:
	SDL_Window* m_Window = nullptr;
	SDL_Renderer* m_Renderer = nullptr;
	SDL_GLContextState* m_GLContext = nullptr;

	//Triangle vertex
	const float m_Vertices[9] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	//Vertex Shader
	//Add file-parsing to make these stand-alone files.
	const char* m_VertexShader = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
};