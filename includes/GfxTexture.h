#pragma once
#include <Defines.h>
#include <GfxHandlerInterface.h>
_VOXEL_BEGIN
class GfxTexture:public GfxHandlerInterface
{
private:
	static GenTexGfxFuncPtr gen_tex_gfx_func2;
	static DestroyGfxAssetFuncPtr del_tex_gfx_func;
	static std::vector <std::vector<std::shared_ptr<GfxTexture>>> tempTextureCache;
	static int current_swapchain_idx;
public:
	
	TextureBaseInfo base_info;
	bool destory_handler_on_dispose = true;
	uint32_t width();
	uint32_t height();
	bool is_srgb();
	GfxTexture() { destory_handler_on_dispose = true; };
	GfxTexture(TextureBaseInfo& base_info);
	GfxTexture(TextureBaseInfo& base_info, void* handler, bool destory_handler_on_dispose) : GfxHandlerInterface(handler), destory_handler_on_dispose(destory_handler_on_dispose) { this->base_info = base_info; }
	static void set_gfx_func(GenTexGfxFuncPtr m_gen_gfx_func, DestroyGfxAssetFuncPtr del_tex_gfx_func)
	{
		GfxTexture::gen_tex_gfx_func2 = m_gen_gfx_func;
		GfxTexture::del_tex_gfx_func = del_tex_gfx_func;
	}
	static void init(int count)
	{
		tempTextureCache.resize(count);
	}
	static void set_current_swapchain_idx(int idx)
	{
		current_swapchain_idx = idx;
	}
	template<class ...Args>
	static std::shared_ptr<GfxTexture> acquire_temp_texture(Args... args)
	{
		auto tex = std::make_shared<GfxTexture>(args...);
		tempTextureCache[current_swapchain_idx].push_back(tex);
		return tex;
	}
	static void clear_temp_cache()
	{
		tempTextureCache[current_swapchain_idx].clear();
	}
	virtual ~GfxTexture();
	
};
_VOXEL_END