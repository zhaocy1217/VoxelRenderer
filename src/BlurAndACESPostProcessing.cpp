#include<BlurAndACESPostProcessing.h>
#include <Resources.h>
_VOXEL_BEGIN
void BlurAndACESPostProcessing::on_render_post(RenderContext& context)
{
	auto temp = GfxTexture::acquire_temp_texture(context.source->base_info);
	context.blit(context.source, temp);
	if (blur)
	{
		auto blur = Resources::get_shader("assets/shaders/blur");
		Material::emptyMat->set_shader(blur);
	}
	else 
	{
		Material::emptyMat->set_shader(Shader::blit);
	}
	context.blit(context.source, temp, Material::emptyMat);
	if (aces)
	{
		auto aces = Resources::get_shader("assets/shaders/aces");
		Material::emptyMat->set_shader(aces);
	}
	else 
	{
		Material::emptyMat->set_shader(Shader::blit);
	}
	context.blit(temp, context.dest, Material::emptyMat);
}
_VOXEL_END