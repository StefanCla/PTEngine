#include "precomp.hpp"

#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"

namespace PT
{

	Mesh::Mesh(std::vector<Vertex> Vertices, std::vector<uint32_t> Indices, std::vector<Texture> Textures)
		: m_Vertices(Vertices)
		, m_Indices(Indices)
		, m_Textures(Textures)
	{
		SetupMesh();
	}

	void Mesh::Draw(PT::Shader& shader)
	{
		uint32_t diffuseNr = 1;
		uint32_t specularNr = 1;

		for (uint32_t i = 0; i < m_Textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string name = m_Textures[i].Type;

			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++);
			}

			shader.SetUniformInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, m_Textures[i].Id);
		}

		//Unbind texture
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Mesh::SetupMesh()
	{
		//Generate buffers
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		//Bind to VAO &VBO
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		//Setup VBO Data
		glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

		//Bind to EBO & setup EBO data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint32_t), &m_Indices[0], GL_STATIC_DRAW);

		//Setup vertex attributes
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);								//Vertices

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));		//Normals

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));	//Texture coords

		//Unbind VAO
		glBindVertexArray(0);
	}

};