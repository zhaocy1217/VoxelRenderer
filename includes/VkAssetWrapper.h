#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include <VkHelper.h>
#include <GfxTexture.h>
_VOXEL_BEGIN
class VkHelper;
class GfxTexture;
class VkBufferWrapper
{
public:
	uint32_t size;
	bool in_recording;
	VkDeviceMemory mem;
	VkBuffer buffer;
};
class VkMeshWrapper
{
public:
	VkBufferWrapper vertex_buffer;
	VkBufferWrapper index_buffer;
	int triangle_size;
	VkMeshWrapper(std::vector<float>& vertexData, std::vector<unsigned int>& indices);
	virtual ~VkMeshWrapper();
};
class VkShaderWrapper
{
public:
	VkShaderModule vert_shader_module = nullptr;
	VkShaderModule frag_shader_module = nullptr;
	VkDescriptorSetLayout layout = nullptr;
	VkPipelineLayout pipeline_layout = nullptr;
	VkVertexInputBindingDescription binding_desc = {};
	std::vector<VkVertexInputAttributeDescription> attr_desc = {};
	size_t id = -1;
	VkShaderWrapper(std::vector<char>& v, std::vector<char>& f,
		ShaderVertexInput& vertexInput,
		std::vector<ShaderBindingObject>& resources_bindings);
	virtual ~VkShaderWrapper();
};
class VkTextureWrapper
{
public:
	VkImage image;
	VkDeviceMemory image_memory;
	VkImageView image_view;
	VkSampler sampler = nullptr;
	VkFormat format;
	glm::uvec2 size;
	VkImageLayout cur_layout = { VkImageLayout ::VK_IMAGE_LAYOUT_UNDEFINED};
	VkTextureWrapper(unsigned char* data, TextureBaseInfo& info);
	VkTextureWrapper(TextureBaseInfo& info);
	static VkTextureWrapper* get_vktexture_from_gfx_tex(GfxTexture* gfx_tex)
	{
		return (VkTextureWrapper*)(gfx_tex->handler);
	}
	VkTextureWrapper();
	~VkTextureWrapper();
};

class VkFrameBufferWrapper
{
private:
	bool destroy_on_dispose;
public:
	std::shared_ptr<GfxTexture> color = nullptr;
	std::shared_ptr<GfxTexture> depth = nullptr;
	VkImageView additional_imageview = nullptr;
	VkFramebuffer vk_fbo = nullptr;
	bool srgb;
	glm::uvec2 size;
	uint32_t msaa = 1;
	VkFrameBufferWrapper(TextureBaseInfo& info);
	VkFrameBufferWrapper(std::shared_ptr<GfxTexture> color, std::shared_ptr<GfxTexture> depth);
	virtual ~VkFrameBufferWrapper();
};
_VOXEL_END