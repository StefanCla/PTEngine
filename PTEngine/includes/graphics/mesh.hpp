#pragma once

#include <glm/glm.hpp>

namespace PT
{
	class Shader;

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture
	{
		uint32_t Id;
		std::string Type;
		std::string Path;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex> Vertices, std::vector<uint32_t> Indices, std::vector<Texture> Textures);

		void Draw(PT::Shader& shader);

	public:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Texture> m_Textures;

	private:
		void SetupMesh();

	private:
		uint32_t m_VAO;
		uint32_t m_VBO;
		uint32_t m_EBO;

	};

};