#include "precomp.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

#include "graphics/renderer.hpp"

namespace PT
{

	Renderer::Renderer()
	{}

	Renderer::~Renderer()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();

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
			printf("RENDERER ERROR: %s\n", SDL_GetError());
			return false;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		m_Window = SDL_CreateWindow("PTEngine", Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

		if (!m_Window)
		{
			printf("RENDERER ERROR: %s\n", SDL_GetError());
			return false;
		}

		SetRelativeMouseMode(true);

		m_GLContext = SDL_GL_CreateContext(m_Window);

		if (!m_GLContext)
		{
			printf("RENDERER ERROR: %s\n", SDL_GetError());
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

		ResizeWindow(Width, Height);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		ImGuiInitialize();

		return true;
	}

	void Renderer::SwapWindow()
	{
		SDL_GL_SwapWindow(m_Window);

		//Set greenish background after swap
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::SetRelativeMouseMode(bool bEnabled)
	{
		SDL_SetWindowRelativeMouseMode(m_Window, bEnabled);
	}

	void Renderer::ResizeWindow(int32_t Width, int32_t Height)
	{
		glViewport(0, 0, Width, Height);
	}

	uint32_t Renderer::SetupVBO()
	{
		uint32_t& PlacedID = m_VBOIDs.emplace_back();
		glGenBuffers(1, &PlacedID);

		//GL_ARRAY_BUFFER = Vertex buffer
		BindVBO(PlacedID);

		glBufferData(GL_ARRAY_BUFFER, sizeof(m_VerticesWithNormalAndTex), m_VerticesWithNormalAndTex, GL_STATIC_DRAW);

		return PlacedID;
	}

	uint32_t Renderer::SetupVAO()
	{
		uint32_t& PlacedID = m_VAOIDs.emplace_back();

		glGenVertexArrays(1, &PlacedID);
		BindVAO(PlacedID);

		return PlacedID;
	}

	void Renderer::SetupEBO()
	{
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_STATIC_DRAW);
	}

	void Renderer::BindVBO(uint32_t& VBOID)
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBOID);
	}

	void Renderer::BindVAO()
	{
		if (!m_VAOIDs.empty())
		{
			glBindVertexArray(m_VAOIDs[0]);
		}
	}

	void Renderer::BindVAO(const uint32_t& VAOID)
	{
		glBindVertexArray(VAOID);
	}

	void Renderer::UnbindVAO()
	{
		glBindVertexArray(0);
	}

	void Renderer::SetupVertexAttrib()
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//glEnableVertexAttribArray(2);

		//glBindBuffer(GL_VERTEX_ARRAY, 0);
	}

	//uint32_t Renderer::SetupTexture(const std::string& FileName, bool bHasAlpha, bool bFlipImage)
	//{
	//	uint32_t TextureID;
	//	glGenTextures(1, &TextureID);
	//	m_TextureID.push_back(TextureID);

	//	glActiveTexture(GL_TEXTURE0 + (m_TextureID.size() - 1));
	//	glBindTexture(GL_TEXTURE_2D, TextureID);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	stbi_set_flip_vertically_on_load(bFlipImage);

	//	const std::string TexturePath = "../resources/textures/";
	//	int Width, Height, ChannelCount;
	//	unsigned char* ImageData = stbi_load((TexturePath + FileName).c_str(), &Width, &Height, &ChannelCount, 0);

	//	if (ImageData)
	//	{
	//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, bHasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, ImageData);
	//		glGenerateMipmap(GL_TEXTURE_2D);
	//	}
	//	else
	//	{
	//		printf("TEXTURE ERROR: Loading in %s was unsuccessful, texture was not loaded in.\n", FileName.c_str());
	//	}

	//	stbi_image_free(ImageData);

	//	return TextureID;
	//}

	void Renderer::ImGuiInitialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplSDL3_InitForOpenGL(m_Window, m_GLContext);
		ImGui_ImplOpenGL3_Init();
	}

	void Renderer::ImGuiNewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void Renderer::ImGuiEndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

};