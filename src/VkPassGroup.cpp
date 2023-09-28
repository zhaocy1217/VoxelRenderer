#include "VkPassGroup.h"
#include <VkRenderer.h>
_VOXEL_BEGIN
void VkPassGroup::blit(VkCommandBuffer vkCmd, std::shared_ptr<GfxTexture> source, std::shared_ptr<GfxTexture> dest, std::shared_ptr<Material> mat)
{
	auto sourceTex = VkTextureWrapper::get_vktexture_from_gfx_tex(source.get());
	auto destTex = VkTextureWrapper::get_vktexture_from_gfx_tex(dest.get());
	{
		if (sourceTex->cur_layout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			VkImageMemBarrier barrier;
			barrier.old_layout = sourceTex->cur_layout;
			barrier.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			barrier.dst_access_mask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
			VkHelper::vk_instance->transit_layout(vkCmd, sourceTex, barrier);
		}
	}
	{
		if (destTex->cur_layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			VkImageMemBarrier barrier{};
			barrier.old_layout = destTex->cur_layout;
			barrier.new_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.src_access_mask = VK_ACCESS_NONE;
			barrier.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkHelper::vk_instance->transit_layout(vkCmd, destTex, barrier);
		}
	}
	
	auto fb = std::make_shared<VkFrameBufferWrapper>(dest, nullptr);
	fb->size = destTex->size;
	fb->msaa = 1;
	fb->srgb = dest->is_srgb();
	auto pass = VkHelper::vk_instance->pool->acquire_render_pass(fb);
	if (sourceTex->sampler == nullptr)
	{
		VkHelper::vk_instance->create_texture_sampler(1, sourceTex->sampler);
	}
	mat->set_texture(Shader::BASE_MAP_NAME_STR, source);
	RenderObjectData rod;
	rod.cd.cameraPosi = glm::zero<glm::vec3>();
	rod.cd.view = glm::identity<glm::mat4>();
	rod.cd.view = glm::translate(rod.cd.view, glm::vec3(0, 0, 0));
	rod.cd.proj = glm::ortho(-(1.0f / 2.0f), 1.0f / 2.0f,
		1.0f / 2.0f, -(1.0f / 2.0f),
		-1.0f, 1.0f);
	rod.cd.proj[1][1] *= -1;
	glm::vec3 pos(-0.5, -0.5, 0);
	glm::vec3 scale(1, 1, 1);
	glm::mat4 translation_m = glm::mat4(1.f, 0, 0, 0, 0, 1., 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1);
	glm::mat4 rotation_m = glm::mat4_cast(glm::identity<glm::quat>());
	glm::mat4 scale_m = glm::mat4(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
	rod.od.model = translation_m * rotation_m * scale_m;
	std::vector<std::shared_ptr<VoxelRendering::VkRenderer>> rs{};
	GfxRenderDataset* dataset = new GfxRenderDataset(Mesh::quad, mat, rod);
	auto r = std::make_shared<VoxelRendering::VkRenderer>(dataset);
	rs.push_back(r);
	pass->begin(vkCmd, rs, true, { 0,0,0,0 }, true, 1);
	pass->end(vkCmd);
	mat->set_texture(Shader::BASE_MAP_NAME_STR, nullptr);
	delete dataset;
}
void VkPassGroup::blit2(VkCommandBuffer vkCmd, std::shared_ptr<GfxTexture> source, std::shared_ptr <GfxTexture> dest)
{
	auto sourceTex = VkTextureWrapper::get_vktexture_from_gfx_tex(source.get());
	auto destTex = VkTextureWrapper::get_vktexture_from_gfx_tex(dest.get());
	{
		if (sourceTex->cur_layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			VkImageMemBarrier barrier;
			barrier.old_layout = sourceTex->cur_layout;
			barrier.new_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.dst_access_mask = VK_ACCESS_TRANSFER_READ_BIT;
			VkHelper::vk_instance->transit_layout(vkCmd, sourceTex, barrier);
		}
	}
	{
		if (destTex->cur_layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			VkImageMemBarrier barrier;
			barrier.old_layout = destTex->cur_layout;
			barrier.new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
			barrier.src_access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			barrier.dst_access_mask = VK_ACCESS_SHADER_READ_BIT;
			VkHelper::vk_instance->transit_layout(vkCmd, destTex, barrier);
		}
	}

	VkImageBlit region;
	memset(&region, 0, sizeof(region));
	region.srcOffsets[1].x = source->base_info.size.x;
	region.srcOffsets[1].y = source->base_info.size.y;
	region.srcOffsets[1].z = 1;
	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.layerCount = 1;
	region.srcSubresource.mipLevel = 0;
	region.dstOffsets[1].x = dest->base_info.size.x;
	region.dstOffsets[1].y = dest->base_info.size.y;
	region.dstOffsets[1].z = 1;
	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.layerCount = 1;
	region.dstSubresource.mipLevel = 0;
	vkCmdBlitImage(vkCmd, sourceTex->image, sourceTex->cur_layout, destTex->image, destTex->cur_layout, 1, &region, VK_FILTER_LINEAR);
}

void VkPassGroup::blit(VkCommandBuffer vkCmd, std::shared_ptr<GfxTexture> source, std::shared_ptr <GfxTexture> dest)
{
	Material::emptyMat->set_shader(Shader::blit);
	blit(vkCmd, source, dest, Material::emptyMat);
}

VoxelRendering::VkPassGroup::VkPassGroup()
{
	auto instance = VoxelRendering::VkHelper::vk_instance;
	if (instance->pool->camera_base_fb == nullptr)
	{
		uint32_t w, h;
		instance->get_window_size(&w, &h);
		TextureBaseInfo info;
		info.size = glm::uvec2(w, h);
		info.srgb = true;
		instance->pool->camera_base_fb = std::make_shared<VkFrameBufferWrapper>(info);
	}
	passes.push_back(VkHelper::vk_instance->pool->acquire_render_pass(instance->pool->camera_base_fb));
}
_VOXEL_END