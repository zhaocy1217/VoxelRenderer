#pragma once
#include<Defines.h>
#include <World.h>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include <Resources.h>
#include <MeshRendererComp.h>
_VOXEL_BEGIN
class GLTFReader
{
private:
	static std::shared_ptr<Texture> GetTex(const aiScene* src_scene, aiMaterial* src_mat, aiTextureType textureType, int idx);
	static aiMatrix4x4 GetMatrix(aiNode* node);
	static void ReadNode(aiNode* node, std::function<void(aiNode*)> func);
public:
	static void Read(World* world, std::string& path);
	static void Read(World* world, std::string&& path);
};
_VOXEL_END