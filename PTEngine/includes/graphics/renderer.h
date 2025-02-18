#pragma once
#include <stdint.h>
#include <string>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <gl/GL.h>

//Add file loading for shaders using fstream (ifstream)

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool SetupWindow(int32_t Width, int32_t Height);
	void ResizeWindow(int32_t Width, int32_t Height);
	void SwapWindow();

	void SetupVBO(); //Vertex Buffer Object
	void SetupVAO(); //Vertex Array Object
	void SetupEBO(); //Element Buffer Object

	void SetupVertexAttrib();

	void BindVAO();
	void UnbindVAO();

	void SetupVertexShader();
	void SetupFragmentShader();
	void SetupShaderProgram();

	void ReadShaderFile(const std::string& File);

public:
	uint32_t m_ShaderProgramID;

private:
	SDL_Window* m_Window = nullptr;
	SDL_Renderer* m_Renderer = nullptr;
	SDL_GLContextState* m_GLContext = nullptr;

	const float m_Vertices[12] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	const uint32_t m_Indices[6] = { 
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	const char* m_VertexShader = "#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* m_FragmentShader = "#version 460 core\n"
		"out vec4 FragColor; \n"
		"void main()\n"
		"{\n"
			"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";
	
	uint32_t m_VertexShaderID;
	uint32_t m_FragmentShaderID;
	uint32_t m_VBO;
	uint32_t m_VAO;
	uint32_t m_EBO;

	char* m_ShaderSource = nullptr;
};