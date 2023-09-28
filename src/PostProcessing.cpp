#pragma once
#include<PostProcessing.h>
#include<Resources.h>
_VOXEL_BEGIN
void PostProcessing::on_render_post(RenderContext& context)
{
	auto temp = GfxTexture::acquire_temp_texture(context.source->base_info);
	context.blit(context.source, temp);
	auto blur = Resources::get_shader("assets/shaders/blur");
	Material::emptyMat->set_shader(blur);
	context.blit(context.source, temp, Material::emptyMat);

	auto aces = Resources::get_shader("assets/shaders/aces");
	Material::emptyMat->set_shader(aces);
	context.blit(temp, context.dest, Material::emptyMat);
}
PostProcessing::~PostProcessing()
{
}
_VOXEL_END

