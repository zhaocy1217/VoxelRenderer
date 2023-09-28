#pragma once
#include <Defines.h>
#include <VkHelper.h>
#include <VkAssetWrapper.h>
#include <VkRenderer.h>
_VOXEL_BEGIN
class VkFrameBufferWrapper;
class FrameBufferCreateInfo;
class VkRenderer;
class VkPass
{
private:
public:
	glm::uvec2 size;
	std::shared_ptr<VkFrameBufferWrapper> fbo;
	VkRenderPass renderPass;
	void begin(VkCommandBuffer vkCmd, std::vector<std::shared_ptr<VkRenderer>>& renderers, bool clearColor, glm::vec4 color, bool clearDepth, float depthValue);
	void end(VkCommandBuffer vkCmd);
	VkPass();
	VkPass(std::shared_ptr<VkFrameBufferWrapper> info);
	void init(std::shared_ptr<VkFrameBufferWrapper> info);
	virtual ~VkPass();
};

_VOXEL_END