#include "precomp.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "graphics/model.hpp"
#include "graphics/shader.hpp"

namespace PT
{

	Model::Model(const char* Path)
	{
		LoadModel(Path);
	}

	void Model::Draw(PT::Shader& shader)
	{
		for (uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			m_Meshes[i].Draw(shader);
		}
	}

	void Model::LoadModel(std::string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR: ASSIMP:" << importer.GetErrorString() << std::endl;
			return;
		}

		m_Directory = path.substr(0, path.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<PT::Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<PT::Texture> textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			PT::Vertex vertex;
			glm::vec3 vert;
			vert.x = mesh->mVertices[i].x;
			vert.y = mesh->mVertices[i].y;
			vert.z = mesh->mVertices[i].z;

			glm::vec3 norm;
			norm.x = mesh->mNormals[i].x;
			norm.y = mesh->mNormals[i].y;
			norm.z = mesh->mNormals[i].z;

			glm::vec2 tex = glm::vec2(0.0f);
			if (mesh->mTextureCoords[0])
			{
				tex.x = mesh->mTextureCoords[0][i].x;
				tex.y = mesh->mTextureCoords[0][i].y;
			}

			vertex.Position = vert;
			vertex.Normal = norm;
			vertex.TexCoords = tex;

			vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<PT::Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");

			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<PT::Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<PT::Texture> textures;

		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool bSkip = false;

			for (uint32_t j = 0; j < m_LoadedTextures.size(); j++)
			{
				if (std::strcmp(m_LoadedTextures[j].Path.data(), str.C_Str()) == 0)
				{
					textures.push_back(m_LoadedTextures[j]);
					bSkip = true;
					break;
				}
			}

			if (!bSkip)
			{
				PT::Texture texture;
				texture.Id = TextureFromFile(str.C_Str(), m_Directory);
				texture.Type = typeName;
				texture.Path = str.C_Str();
				textures.push_back(texture);
				m_LoadedTextures.push_back(texture);
			}
		}

		return textures;
	}

	uint32_t Model::TextureFromFile(const char* path, const std::string& dir)
	{
		std::string filename = std::string(path);
		filename = m_Directory + '/' + filename;

		uint32_t textureID;
		glGenTextures(1, &textureID);

		int width;
		int height;
		int nrComponents;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
			{
				format = GL_RED;
			}
			else if (nrComponents == 3)
			{
				format = GL_RGB;
			}
			else if (nrComponents == 4)
			{
				format = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			std::cout << "ERROR: Failed to lead model texture at path: " << path << std::endl;
		}

		stbi_image_free(data);

		return textureID;
	}

};