#pragma once
#include <glad/glad.h>
#include <stdint.h>
#include <string>

enum class EShaderType
{
	Vertex,
	Fragment
};

class Shader
{
public:
	Shader();
	~Shader();

	const uint32_t GetShaderProgram() const; //std:expected?
	bool LoadShader(const std::string& FileName, EShaderType ShaderType);
	void Use();

	//Uniform
	void SetUniformBool(const std::string& Name, bool Value) const;
	void SetUniformInt(const std::string& Name, int Value) const;
	void SetUniformFloat(const std::string& Name, float Value) const;

private:
	uint32_t m_ShaderProgramID = 9999;

};