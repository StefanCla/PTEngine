#include "graphics/renderer.h"
#include <stdio.h>

Renderer2::Renderer2()
{

}

Renderer2::~Renderer2()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteShader(m_ShaderProgramID);

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
	glGenBuffers(1, &m_VBO);

	//GL_ARRAY_BUFFER = Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_STATIC_DRAW);
}

void Renderer2::SetupVAO()
{
	glGenVertexArrays(1, &m_VAO);
	BindVAO();
}

void Renderer2::SetupEBO()
{
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW);
}

void Renderer2::SetupVertexShader()
{
	m_VertexShaderID = glCreateShader(GL_VERTEX_SHADER);

	//Add shader source
	glShaderSource(m_VertexShaderID, 1, &m_VertexShader, NULL);

	//Dynamically compile shader
	glCompileShader(m_VertexShaderID);

	int32_t CompileSuccess;
	char Log[512];
	glGetShaderiv(m_VertexShaderID, GL_COMPILE_STATUS, &CompileSuccess);

	if (!CompileSuccess)
	{
		glGetShaderInfoLog(m_VertexShaderID, 512, NULL, Log);
		printf(Log);
	}

	//https://learnopengl.com/Getting-started/Hello-Triangle
}

void Renderer2::SetupFragmentShader()
{
	m_FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_FragmentShaderID, 1, &m_FragmentShader, NULL);
	glCompileShader(m_FragmentShaderID);

	int32_t CompileSuccess;
	char Log[512];
	glGetShaderiv(m_FragmentShaderID, GL_COMPILE_STATUS, &CompileSuccess);

	if (!CompileSuccess)
	{
		glGetShaderInfoLog(m_FragmentShaderID, 512, NULL, Log);
		printf(Log);
	}
}

void Renderer2::SetupShaderProgram()
{
	m_ShaderProgramID = glCreateProgram();

	glAttachShader(m_ShaderProgramID, m_VertexShaderID);
	glAttachShader(m_ShaderProgramID, m_FragmentShaderID);
	glLinkProgram(m_ShaderProgramID);

	int32_t LinkSuccess;
	char Log[512];
	glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &LinkSuccess);
	if (!LinkSuccess)
	{
		glGetProgramInfoLog(m_ShaderProgramID, 512, NULL, Log);
		printf(Log);
	}

	glDeleteShader(m_VertexShaderID);
	glDeleteShader(m_FragmentShaderID);
}

void Renderer2::BindVAO()
{
	glBindVertexArray(m_VAO);
}

void Renderer2::UnbindVAO()
{
	glBindVertexArray(0);
}

void Renderer2::SetupVertexAttrib()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_VERTEX_ARRAY, 0);
}