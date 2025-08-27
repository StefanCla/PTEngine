#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "graphics/mesh.hpp"

namespace PT
{
	class Shader;

	class Model
	{
	public:
		Model(const char* Path);
		
		void Draw(PT::Shader& shader);

	private:
		void LoadModel(std::string Path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<PT::Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		uint32_t TextureFromFile(const char* path, const std::string& dir);

	private:
		std::vector<PT::Mesh> m_Meshes;
		std::string m_Directory;
		std::vector<PT::Texture> m_LoadedTextures;

	};

};