#include "VkPass.h"
_VOXEL_BEGIN
void VkPass::begin(VkCommandBuffer vkCmd, std::vector<std::shared_ptr<VkRenderer>>& renderers, 
	bool clearColor, glm::vec4 color, bool clearDepth, float depthValue)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = fbo->vk_fbo;
	renderPassInfo.renderArea.offset = { 0, 0 };
	VkExtent2D ext;
	ext.width = size.x;
	ext.height = size.y;
	renderPassInfo.renderArea.extent = ext;
	std::vector<VkClearValue> clearValues;
	if (clearColor)
	{
		VkClearValue colorClearInfo;
		colorClearInfo.color = { {color.r, color.g, color.b, color.a} };
		clearValues.push_back(colorClearInfo);
	}
	if (clearDepth)
	{
		VkClearValue depthClearInfo;
		depthClearInfo.depthStencil = { depthValue, 0 };
		clearValues.push_back(depthClearInfo);
	}
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(vkCmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	for (auto VkRenderer : renderers)
	{
		VkRenderer->draw(vkCmd, renderPass);
	}
}
void VkPass::end(VkCommandBuffer vkCmd)
{
	vkCmdEndRenderPass(vkCmd);
}
VkPass::VkPass()
{
}
VoxelRendering::VkPass::VkPass(std::shared_ptr<VkFrameBufferWrapper> info)
{
	init(info);
}
void VkPass::init(std::shared_ptr<VkFrameBufferWrapper> info)
{
	size = info->size;
	auto instance = VoxelRendering::VkHelper::vk_instance;
	PassCreationInfo ci;
	ci.msaa = info->msaa;
	if (info->color != nullptr || info->depth!=nullptr)
	{
		ci.flags = ((info->color != nullptr) ? (uint32_t)ATTACHMENT_TYPE::COLOR : 0)
			| ((info->depth != nullptr) ? (uint32_t)ATTACHMENT_TYPE::DEPTH : 0);
		if (info->color == nullptr)
		{
			ci.colorFormat = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
		}
		else
		{
			auto t = (VkTextureWrapper*)(info->color->handler);
			ci.colorFormat = t->format;
		}
		instance->create_render_pass(VkPass::renderPass, ci);
		instance->rebind_framebuffer_with_pass(VkPass::renderPass, info);
		this->fbo = info;
	}
	else
	{
		std::runtime_error("color or depth buffer not exsit!");
	}
}
VoxelRendering::VkPass::~VkPass()
{
	auto instance = VoxelRendering::VkHelper::vk_instance;
	instance->destroy_render_pass(VkPass::renderPass);
	if (fbo != nullptr)
	{
		instance->destroy_frame_buffer_object(fbo->vk_fbo);//destory frambuffer but texture not destory
		fbo->vk_fbo = nullptr;
	}
}

_VOXEL_END