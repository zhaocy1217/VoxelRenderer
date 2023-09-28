#pragma once
#include <GfxTexture.h>
_VOXEL_BEGIN
GenTexGfxFuncPtr GfxTexture::gen_tex_gfx_func2=nullptr;
DestroyGfxAssetFuncPtr GfxTexture::del_tex_gfx_func=nullptr;
std::vector<std::vector<std::shared_ptr<GfxTexture>>>  GfxTexture::tempTextureCache;
int GfxTexture::current_swapchain_idx;
GfxTexture::~GfxTexture()
{
	if (destory_handler_on_dispose && handler!=nullptr)
	{
		del_tex_gfx_func(handler);
		handler = nullptr;
	}
}
uint32_t GfxTexture::width()
{
	return base_info.size.x;
}
uint32_t GfxTexture::height()
{
	return base_info.size.y;
}
bool GfxTexture::is_srgb()
{
	return base_info.srgb;
}
GfxTexture::GfxTexture(TextureBaseInfo& base_info)
{
	this->base_info = base_info;
	handler = gen_tex_gfx_func2(base_info);
}
_VOXEL_END

