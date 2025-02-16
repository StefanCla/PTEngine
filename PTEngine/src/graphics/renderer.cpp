#include "graphics/renderer.h"
#include <stdio.h>

Renderer2::Renderer2()
{

}

Renderer2::~Renderer2()
{
	SDL_GL_DestroyContext(m_GLContext);
}

bool Renderer2::SetupWindow(int32_t Width, int32_t Height)
{
	if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO))
	{
		printf("Init Err: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	m_Window = SDL_CreateWindow("PTEngine", Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!m_Window)
	{
		printf("Window Err: %s\n", SDL_GetError());
		return false;
	}

	m_GLContext = SDL_GL_CreateContext(m_Window);
	
	if (!m_GLContext)
	{
		printf("Context Err: %s\n", SDL_GetError());
		return false;
	}

	int32_t MajorVer, MinorVer = 0;

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &MajorVer);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &MinorVer);

	printf("Major version: %i, Minor version: %i\n", MajorVer, MinorVer);

	if (MajorVer != 4 && MinorVer != 6)
	{
		//Do something else
		SDL_GL_DestroyContext(m_GLContext);
		return false;
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		return false;
	}

	return true;
}

void Renderer2::SwapWindow()
{
	/*glClear*/

	SDL_GL_SwapWindow(m_Window);
}

void Renderer2::ResizeWindow(int32_t Width, int32_t Height)
{
	glViewport(0, 0, Width, Height);
}

void Renderer2::SetupVBO()
{
	uint32_t VBO;
	glGenBuffers(1, &VBO);

	//GL_ARRAY_BUFFER = Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_STATIC_DRAW);
}

void Renderer2::SetupVertexShader()
{
	uint32_t VertexShader;
	VertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Add shader source
	glShaderSource(VertexShader, 1, &m_VertexShader, NULL);

	//Dynamically compile shader
	glCompileShader(VertexShader);

	int32_t CompileSuccess;
	char Log[512];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &CompileSuccess);

	if (!CompileSuccess)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, Log);
		printf(Log);
	}

	//https://learnopengl.com/Getting-started/Hello-Triangle
}