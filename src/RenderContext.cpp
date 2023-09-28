#include<RenderContext.h>
_VOXEL_BEGIN
BlitFunc RenderContext::blit_func = nullptr;
Blit2Func RenderContext::blit2_func = nullptr;
void RenderContext::blit(std::shared_ptr<GfxTexture> source, std::shared_ptr<GfxTexture> dest, std::shared_ptr<Material> mat)
{
	blit_func(source, dest, mat);
}
void RenderContext::blit(std::shared_ptr<GfxTexture> source, std::shared_ptr<GfxTexture> dest)
{
	blit2_func(source, dest);
}
_VOXEL_END