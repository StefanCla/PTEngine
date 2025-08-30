#include "precomp.hpp"

#include <iostream>

#include "graphics/renderer.hpp" //for stb_image

#include "graphics/model.hpp"
#include "graphics/camera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "stb_image.h"

namespace PT
{

	Model::Model(const char* Path, PT::Camera* Camera)
		: m_Camera(Camera)
	{
		m_ModelShader.LoadShader("model.vert", PT::EShaderType::Vertex);
		m_ModelShader.LoadShader("model.frag", PT::EShaderType::Fragment);
		LoadModel(Path);
	}

	void Model::Draw()
	{
		SetModelShaderUniforms();

		for (uint32_t i = 0; i < m_Meshes.size(); i++)
		{
			m_Meshes[i].Draw(m_ModelShader);
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

	//These values are currently hardcoded on purpose.
	void Model::SetModelShaderUniforms()
	{
		m_ModelShader.Use();

		glm::mat4 View = glm::mat4(1.0f);
		glm::vec3 CameraPos = glm::vec3(1.0f);
		if (m_Camera)
		{
			View = m_Camera->CalculateViewMatrix();
			CameraPos = m_Camera->GetCameraPos();
		}

		uint32_t ViewID = glGetUniformLocation(m_ModelShader.GetShaderProgram(), "View");
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(View));

		uint32_t ProjectionID = glGetUniformLocation(m_ModelShader.GetShaderProgram(), "Projection");
		glm::mat4 Projection;
		Projection = glm::perspective(glm::radians(65.0f), static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT), 0.1f, 100.0f);
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));

		uint32_t ModelID = glGetUniformLocation(m_ModelShader.GetShaderProgram(), "Model");
		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
		//Model = glm::scale(Model, glm::vec3(1.0f, 1.4f, 1.0f));
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(Model));

		//Setup all materials & lighting
		m_ModelShader.SetUniformInt("material.diffuse", 0);
		m_ModelShader.SetUniformInt("material.specular", 1);
		m_ModelShader.SetUniformVec3("viewPos", CameraPos);
		m_ModelShader.SetUniformFloat("material.shininess", 32.0f);

		m_ModelShader.SetUniformVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		m_ModelShader.SetUniformVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ModelShader.SetUniformVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		m_ModelShader.SetUniformVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		// point light 1
		m_ModelShader.SetUniformVec3("pointLights[0].position", glm::vec3(0.7f, 0.2f, 2.0f));
		m_ModelShader.SetUniformVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ModelShader.SetUniformVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_ModelShader.SetUniformVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.SetUniformFloat("pointLights[0].constant", 1.0f);
		m_ModelShader.SetUniformFloat("pointLights[0].linear", 0.09f);
		m_ModelShader.SetUniformFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		m_ModelShader.SetUniformVec3("pointLights[1].position", glm::vec3(2.3f, -3.3f, -4.0f));
		m_ModelShader.SetUniformVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ModelShader.SetUniformVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_ModelShader.SetUniformVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.SetUniformFloat("pointLights[1].constant", 1.0f);
		m_ModelShader.SetUniformFloat("pointLights[1].linear", 0.09f);
		m_ModelShader.SetUniformFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		m_ModelShader.SetUniformVec3("pointLights[2].position", glm::vec3(-4.0f, 2.0f, -12.0f));
		m_ModelShader.SetUniformVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ModelShader.SetUniformVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_ModelShader.SetUniformVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.SetUniformFloat("pointLights[2].constant", 1.0f);
		m_ModelShader.SetUniformFloat("pointLights[2].linear", 0.09f);
		m_ModelShader.SetUniformFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		m_ModelShader.SetUniformVec3("pointLights[3].position", glm::vec3(0.0f, 0.0f, -3.0f));
		m_ModelShader.SetUniformVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		m_ModelShader.SetUniformVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		m_ModelShader.SetUniformVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		m_ModelShader.SetUniformFloat("pointLights[3].constant", 1.0f);
		m_ModelShader.SetUniformFloat("pointLights[3].linear", 0.09f);
		m_ModelShader.SetUniformFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		m_ModelShader.SetUniformBool("bUseSpotLight", false);
	}

};