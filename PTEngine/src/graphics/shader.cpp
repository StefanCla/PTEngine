#include "precomp.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

#include "graphics/shader.hpp"

namespace PT
{

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
	
	void Shader::SetUniformVec3(const std::string& Name, const glm::vec3& Value) const
	{
		glUniform3fv(glGetUniformLocation(m_ShaderProgramID, Name.c_str()), 1, &Value[0]);
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
			printf("SHADER ERROR: Unknown shader type has been specified. Unable to load in shader.\n");
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
			printf("SHADER ERROR: Unable to open file %s\n", FileName.c_str());
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
			printf("SHADER ERROR: Failed to compile shader %s, report: %s\n", FileName.c_str(), infoLog);
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
			printf("SHADER ERROR: Failed to link shader program, report: %s\n", infoLog);
			return false;
		}

		glDeleteShader(ShaderID);

		return true;
	}

	void Shader::Use()
	{
		glUseProgram(m_ShaderProgramID);
	}

};