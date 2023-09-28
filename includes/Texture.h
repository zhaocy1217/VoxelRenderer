#pragma once
#include <string>
#include <Defines.h>
#include <memory>
#include "AssetItem.h"
#include "VkHelper.h"
#include<VkAssetWrapper.h>
#include<GfxTexture.h>
_VOXEL_BEGIN
class Texture :AssetItem, public GfxTexture
{
private:
	static GenRawDataTexGfxFuncPtr gen_gfx_func;
	static DestroyGfxAssetFuncPtr destroy_raw_data_gfx_tex_func;
	static std::shared_ptr<Texture> load_from_uncompressed(std::shared_ptr<Texture> tex, unsigned char* data, unsigned int w, unsigned int h, bool readable, bool srgb, bool cubemap);
public:
	static std::shared_ptr<Texture> white;
	static std::shared_ptr<Texture> black;
	bool readable = false;
	unsigned char* data = nullptr;
	unsigned char* cubemap_data = nullptr;
	Texture(const std::string& path);
	Texture();
	static bool is_srgb_texture(std::string& name);
	static std::shared_ptr<Texture> load(const std::string& path, bool readable, bool srgb, bool cubemap);
	static std::shared_ptr<Texture> load(const unsigned char* data, unsigned int length, bool readable, bool srgb, bool cubemap);
	static std::shared_ptr<Texture> gen(unsigned char* data, unsigned int width, unsigned int height, bool srgb);
	static void set_rawdata_texture_gfx_func(GenRawDataTexGfxFuncPtr m_gen_gfx_func, DestroyGfxAssetFuncPtr del_func)
	{
		gen_gfx_func = m_gen_gfx_func;
		destroy_raw_data_gfx_tex_func = del_func;
	}
	void* gen_gfx_handler();
	virtual ~Texture();
};
_VOXEL_END