#pragma once
#include "Defines.h"
#include "shader.h"
#include <VkHelper.h>
#include <map>
#pragma warning( disable : 26812 )
_VOXEL_BEGIN
class VkFrameBufferWrapper;
class VkPassGroup;
class VkShaderWrapper;
class VkBufferWrapper;
class VkHelper;
class VkPass;
class FrameBufferCreateInfo;
class VkDescriptorSetCacheWrapper
{
public :
	VkDescriptorSet desc_set;
	bool used;
};
class VkPool
{
private:
	int swapchain_count;
	std::map<std::size_t, VkDescriptorSetLayout> layoutCache;
	std::map<std::size_t, VkDescriptorPool> descriptorPoolCache;
	std::vector<std::map<long, std::vector<VkDescriptorSetCacheWrapper>>> descriptorSetCache;
	std::vector<VkCommandBuffer> commandBufferCache;
	std::vector<std::vector<std::shared_ptr<VkPass>>> renderPassCache;
	std::vector<VkPassGroup*> renderGroupsCache;
	std::vector <std::map<intptr_t, VkPipeline>> pipelineCache;
	std::list<VkBufferWrapper> bufferCache;
	VkDescriptorPool descriptorPool;
public:
	std::shared_ptr<VkFrameBufferWrapper> camera_base_fb;
	std::size_t vec_hash(std::vector<ShaderBindingObject> const& vec) const;
	void cleanup();
	VkDescriptorSetLayout acquire_layout(std::vector<ShaderBindingObject>& resources_bindings);
	VkDescriptorSet acquire_descriptor_set(VkDescriptorSetLayout layout, size_t id, bool* cached);
	void acquire_command_buffer(VkCommandBuffer* cb);
	void acquire_render_group(VkPassGroup** rg);
	void acquire_pipeline(VkRenderPass pass, VkShaderWrapper& Shader, VkPipeline* pipeline, bool trans, uint32_t msaa);
	std::shared_ptr<VkPass> acquire_render_pass(std::shared_ptr<VkFrameBufferWrapper> info);
	VkBufferWrapper acquire_buffer2(uint32_t size);
	VkDescriptorPool get_descriptor_pool();
	template <typename T>
	VkBufferWrapper acquire_buffer(T& ubo)
	{
		VkBufferWrapper vkb = acquire_buffer2(sizeof(ubo));
		VkHelper::vk_instance->update_uniform_buffer_obj(ubo, vkb.mem);
		return vkb;
	};
	void refresh();
	VkPool(size_t swapchainCount);
};

_VOXEL_END

