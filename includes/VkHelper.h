
#pragma once
#include "vulkan/vulkan_core.h"
#include "Defines.h"
#include "VkPassGroup.h"
#include <iostream>
#include<memory>
#include <fstream>
#include <VkRenderer.h>
#include <VkPool.h>
#include <chrono>
#include <PostProcessing.h>
#include<MemAllocator.h>
//#define NDEBUG
_VOXEL_BEGIN
class Material;
class VkRenderer;
class GObject;
class VkPool;
class VkPassGroup;
class PostProcessing;
class VkHelper
{
private:
	VkDebugUtilsMessengerEXT debug_messenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	VkSwapchainKHR swapchain;
	std::vector<std::shared_ptr<GfxTexture>> swapchain_textures;
	VkFormat swapchain_image_format;
	VkFormat depth_format;
	VkExtent2D swapChain_extent;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> in_flight_fences;
	std::vector<VkFence> images_in_flight;
	size_t current_swapchain_idx = 0;
	uint32_t acquired_swapchain_idx;
	VkInstance instance;
	VkPass* gui_render_pass;
	VkAllocationCallbacks* allocator_callback;
	const int MAX_FRAMES_IN_FLIGHT = 1;
#ifdef VALIDATION_LAYER
	const bool enable_validation_layers = true;
#else
	const bool enable_validation_layers = false;
#endif
	bool is_cleaningup = false;
	std::function<void(VkInstance, VkSurfaceKHR*)> create_surface_func;
	std::function<const char** (uint32_t*)> get_ext_func;
	std::function<void(uint32_t*, uint32_t*)> get_size_func;

	const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	VkCommandBuffer begin_single_time_commands();
	void end_single_time_commands(VkCommandBuffer command_buffer);
	void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, int layerCount);
	void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, std::vector<VkBufferImageCopy>& bufferCopyRegions);
	void generate_mipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, int layerCount);
	void initImGui(std::function<void()> setup_imgui_func);
public:
	VkPool* pool;
	static VkHelper* vk_instance;
	static void start_vk(VkStartInfo& start_info);
	void wait_device_idle();
	void cleanup();
	void init();
	void render(std::vector<GfxRenderDataset*>& renderers, PostProcessing* pp, std::function<void()> gui);
	void create_cmd_buffer(VkCommandBuffer& command_buffer);
	VkCommandBuffer create_cmd_buffer(std::vector<std::shared_ptr<VkRenderer>>& current_renderer, PostProcessing* pp, std::function<void()> gui);
	void create_sync_objects();
	void create_image(VkImage& textureImage, VkDeviceMemory& textureImageMemory, unsigned char* pixels, unsigned int texWidth, unsigned int texHeight, unsigned int channel, unsigned int mipLevels,bool cubemap, VkFormat format);
	void create_image(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& image_memory, int arrayLayers);
	void create_image_view(VkImage& image, VkImageView& image_view, VkImageViewType imageViewType, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	std::shared_ptr<VkFrameBufferWrapper> create_frame_buffer(glm::uvec2 size, VkFormat colorFormat, VkFormat depth_format, uint32_t msaaSamples, bool srgb, VkRenderPass& renderPass, VkImageView& additional_imageview);
	void rebind_framebuffer_with_pass(VkRenderPass renderPass, std::shared_ptr<VkFrameBufferWrapper> fb);
	void create_texture_sampler(uint32_t mipLevels, VkSampler& textureSampler);
	void create_vertex_buffer(std::vector<float>& vertices, VkBuffer& vertex_buffer, VkDeviceMemory& vertexBufferMemory);
	void create_shader_module(const std::vector<char>& code, VkShaderModule& shaderModule);
	void create_index_buffer(std::vector<unsigned int>& indices, VkBuffer& index_buffer, VkDeviceMemory& indexBufferMemory);
	void create_render_pass(VkRenderPass& rp, PassCreationInfo& creationInfo);
	void create_pipeline_layout(VkDescriptorSetLayout layout, VkPipelineLayout* pipeline_layout);
	void create_graphics_pipeline(VkShaderWrapper* Shader, VkRenderPass& pass, bool isTransparent, uint32_t msaa, VkPipeline* pipeline);
	void create_descriptor_set_layout(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayout& descriptorSetLayout);
	void create_descriptor_pool(VkDescriptorPoolCreateInfo& poolInfo, VkDescriptorPool& descriptorPool);
	void create_descriptor_sets(VkDescriptorPool& descriptorPool, VkDescriptorSet& descriptorSet, VkDescriptorSetLayout& layout);
	void update_descriptor_sets(
		uint32_t                                    descriptorWriteCount,
		const VkWriteDescriptorSet* pDescriptorWrites,
		uint32_t                                    descriptorCopyCount,
		const VkCopyDescriptorSet* pDescriptorCopies);
	void transit_layout(VkCommandBuffer vkCmd, VkTextureWrapper* tex, VkImageMemBarrier& barrier);
	void destroy_buffer(VkBuffer buffer, VkDeviceMemory mem);
	void destroy_frame_buffer_object(VkFramebuffer fbo);
	void destroy_image(VkImage image, VkImageView view, VkSampler sampler, VkDeviceMemory mem);
	void destroy_render_pass(VkRenderPass rp);
	void destroy_pipeline(VkPipeline layout);
	void destroy_pipeline_layout(VkPipelineLayout layout);
	void destroy_shader_module(VkShaderModule layout);
	void destroy_descriptor_set_layout(VkDescriptorSetLayout layout);
	void destroy_descriptor_pool(VkDescriptorPool pool);
	void get_window_size(uint32_t* w, uint32_t* h);
	uint32_t get_swapchain_count();
	uint32_t get_current_swapchain_index();
	VkFormat get_swapchain_image_format();
	VkFormat get_depth_format();
	void create_uniform_buffer2(unsigned long long size, VkBuffer& buffer, VkDeviceMemory& mem) {
		VkDeviceSize bufferSize = size;
		VkHelper::create_buffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, mem);
	}
	template <typename T>
	void create_uniform_buffer(T& ubo, VkBuffer& buffer, VkDeviceMemory& ubo_memory)
	{
		void* data;
		unsigned long long size = sizeof(ubo);
		create_uniform_buffer2(size, buffer, ubo_memory);
		vkMapMemory(device, ubo_memory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, ubo_memory);
	}
	template <typename T>
	void update_uniform_buffer_obj(T& ubo, VkDeviceMemory& ubo_memory)
	{
		void* data;
		vkMapMemory(device, ubo_memory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, ubo_memory);
	}
	template <typename T>
	void update_uniform_buffer_array_obj(std::vector<T>& ubo, VkDeviceMemory& ubo_memory)
	{
		void* data;
		auto size = sizeof(ubo[0])*ubo.size();
		vkMapMemory(device, ubo_memory, 0, size, 0, &data);
		memcpy(data, ubo.data(), size);
		vkUnmapMemory(device, ubo_memory);
	}
	template <typename T>
	void update_uniform_buffer_array_obj(T* ubo, int len, VkDeviceMemory& ubo_memory)
	{
		void* data;
		auto size = sizeof(T) * len;
		vkMapMemory(device, ubo_memory, 0, size, 0, &data);
		memcpy(data, ubo, size);
		vkUnmapMemory(device, ubo_memory);
	}

};
_VOXEL_END