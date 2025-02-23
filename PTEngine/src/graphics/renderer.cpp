#include "precomp.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "graphics/renderer.h"

namespace PT
{

	Renderer::Renderer()
	{}

	Renderer::~Renderer()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		glDeleteShader(m_ShaderProgramID);

		SDL_GL_DestroyContext(m_GLContext);
	}

	bool Renderer::SetupWindow(int32_t Width, int32_t Height)
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

	void Renderer::SwapWindow()
	{
		SDL_GL_SwapWindow(m_Window);

		//Set greenish background after swap
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::ResizeWindow(int32_t Width, int32_t Height)
	{
		glViewport(0, 0, Width, Height);
	}

	void Renderer::SetupVBO()
	{
		glGenBuffers(1, &m_VBO);

		//GL_ARRAY_BUFFER = Vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_STATIC_DRAW);
	}

	void Renderer::SetupVAO()
	{
		glGenVertexArrays(1, &m_VAO);
		BindVAO();
	}

	void Renderer::SetupEBO()
	{
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW);
	}

	void Renderer::BindVAO()
	{
		glBindVertexArray(m_VAO);
	}

	void Renderer::UnbindVAO()
	{
		glBindVertexArray(0);
	}

	void Renderer::SetupVertexAttrib()
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_VERTEX_ARRAY, 0);
	}

	void Renderer::SetupTexture()
	{
		uint32_t Texture;
		glGenTextures(1, &Texture);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int Width, Height, NrChannels;
		unsigned char* ImageData = stbi_load("../resources/textures/wall.jpg", &Width, &Height, &NrChannels, 0);
		if (ImageData)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, ImageData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			//Log error
		}

		stbi_image_free(ImageData);
		


		uint32_t Texture1;
		glGenTextures(1, &Texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);
		unsigned char* ImageData2 = stbi_load("../resources/textures/awesomeface.png", &Width, &Height, &NrChannels, 0);
		if (ImageData2)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData2);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			//Log error
		}

		stbi_image_free(ImageData2);

	}

};