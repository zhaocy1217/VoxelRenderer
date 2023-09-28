#pragma once
#include <string>
#include <map>
#include <string>
#include<Defines.h>
#include <Texture.h>
#include <Shader.h>
#include <AssetItem.h>
#include<VkHelper.h>
#include<Util.h>
#include<GfxRenderDataset.h>
#include <assimp/material.h>
_VOXEL_BEGIN
class Shader;
class GfxTexture;
class Material :AssetItem
{
private:
	 static const std::string MAT_SHADER_STR;
	 static const std::string MAT_RENDER_TYPE_STR;
	 static const std::string MAT_OPAQUE_STR;
	 static const std::string MAT_TEX_PARAM_STR;
	 static const std::string MAT_FLOAT_PARAM_STR;
	 static const std::string MAT_PROPERTY_STR;
	std::map<std::string, std::shared_ptr<GfxTexture>> texture_params;
	std::map<std::string, std::vector<float>> float_params;
public:
	static const std::string DEFAULT_SKYBOX_MAP;
	static const std::string DEFAULT_SKYBOX_PATH;
	static std::map<std::string, std::vector<aiTextureType>> material_tex_finder;
	static std::shared_ptr<Material> emptyMat;
	RenderType render_type;
	std::shared_ptr<Shader> shader;
	std::map<std::string, std::vector<float>> ubo_buffer_data;
	const std::string material_path;
	int cullMode;
	void set_shader(std::shared_ptr<VoxelRendering::Shader> newShader);
	std::shared_ptr<GfxTexture> get_texture(std::string& tex);
	GfxHandlerInterface* get_gfx_tex_handler(std::string& tex);
	void set_texture(const std::string& texName, std::shared_ptr<GfxTexture> tex);
	Material(const std::string& materialPath);
	Material();
	~Material();
	static std::shared_ptr<Material> load(const std::string& materialPath);
	static std::shared_ptr<Material> serialize(char* mat_str, int& length, std::shared_ptr<Material> mat);

};
_VOXEL_END