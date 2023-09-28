#pragma once
#include<Defines.h>
#include <GfxTexture.h>
#include <Material.h>
_VOXEL_BEGIN
class GfxTexture;
class Material;
class RenderContext
{
private:
	static BlitFunc blit_func;
	static Blit2Func blit2_func;
public:
	std::shared_ptr<GfxTexture> source;
	std::shared_ptr<GfxTexture> dest;
	void blit(std::shared_ptr<GfxTexture> source, std::shared_ptr<GfxTexture> dest, std::shared_ptr<Material> mat);
	void blit(std::shared_ptr<GfxTexture> source, std::shared_ptr<GfxTexture> dest);
	static void set_blit_func(BlitFunc blit_func, Blit2Func blit2_func)
	{
		RenderContext::blit_func = blit_func;
		RenderContext::blit2_func = blit2_func;
	}
};
_VOXEL_END