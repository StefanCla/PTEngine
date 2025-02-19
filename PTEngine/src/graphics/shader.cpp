#include "graphics/shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader()
{}

Shader::~Shader()
{}

const uint32_t Shader::GetShaderProgram() const
{
	return m_ShaderProgramID;
}

void Shader::SetUniformBool(const std::string& Name, bool Value) const
{
	glUniform1i(glGetUniformLocation(m_ShaderProgramID, Name.c_str()), static_cast<int>(Value));
}

void Shader::SetUniformInt(const std::string& Name, int Value) const
{
	glUniform1i(glGetUniformLocation(m_ShaderProgramID, Name.c_str()), Value);
}

void Shader::SetUniformFloat(const std::string& Name, float Value) const
{
	glUniform1f(glGetUniformLocation(m_ShaderProgramID, Name.c_str()), Value);
}

bool Shader::LoadShader(const std::string& FileName, EShaderType ShaderType)
{
	std::string ShaderPath;
	std::string ShaderSource;
	GLuint GLShaderType;

	std::ifstream ShaderFile;
	ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	switch (ShaderType)
	{
		case EShaderType::Vertex:
		{
			ShaderPath = "../resources/shaders/vertex/";
			GLShaderType = GL_VERTEX_SHADER;
		}
		break;
		case EShaderType::Fragment:
		{
			ShaderPath = "../resources/shaders/fragment/";
			GLShaderType = GL_FRAGMENT_SHADER;
		}
		break;
		default:
		{
			//Log error
			return false;
		}
		break;
	}

	try
	{
		ShaderFile.open(ShaderPath + FileName);
		std::stringstream ShaderStream;

		ShaderStream << ShaderFile.rdbuf();

		ShaderFile.close();

		ShaderSource = ShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error" << std::endl;
		return false;
	}

	//Compile
	uint32_t ShaderID;
	int Success;
	char infoLog[512];
	const char* ShaderSource_C = ShaderSource.c_str();

	ShaderID = glCreateShader(GLShaderType);
	glShaderSource(ShaderID, 1, &ShaderSource_C, NULL);
	glCompileShader(ShaderID);

	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Success);
	if (!Success)
	{
		glGetShaderInfoLog(ShaderID, 512, NULL, infoLog);
		//Log error
		return false;
	}

	if (m_ShaderProgramID == 9999)
	{
		m_ShaderProgramID = glCreateProgram();
	}

	glAttachShader(m_ShaderProgramID, ShaderID);
	glLinkProgram(m_ShaderProgramID);

	glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(m_ShaderProgramID, 512, NULL, infoLog);
		//Log error
		return false;
	}

	glDeleteShader(ShaderID);

	return true;
}

void Shader::Use()
{
	glUseProgram(m_ShaderProgramID);
}