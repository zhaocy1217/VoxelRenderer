#include "VkPool.h"
#pragma warning( disable : 26812 )
_VOXEL_BEGIN
std::size_t VkPool::vec_hash(std::vector<ShaderBindingObject> const& vec) const {
	std::size_t seed = vec.size();
	for (auto& i : vec) {
		seed ^= (std::size_t)i.binding_type + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	seed ^= 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed;
}

VkDescriptorType get_descriptor_type(BindingTypes bt)
{
	if (bt == BindingTypes::UNIFORMBUFFER_VERTEX || bt == BindingTypes::UNIFORMBUFFER_FRAGMENT)
	{
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}
	else if (bt == BindingTypes::IMAGE_SAMPLER)
	{
		return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	}
	return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

VkDescriptorSetLayout VkPool::acquire_layout(std::vector<ShaderBindingObject>& resources_bindings)
{
	std::size_t hash = vec_hash(resources_bindings);
	auto res = layoutCache.find(hash);
	if (res != layoutCache.end()) {

		return res->second;
	}
	int bindingIdx = 0;
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	for (auto& binding : resources_bindings)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = bindingIdx;
		layoutBinding.descriptorCount = 1;
		layoutBinding.descriptorType = get_descriptor_type(binding.binding_type);
		layoutBinding.pImmutableSamplers = nullptr;
		layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		layoutBindings.push_back(layoutBinding);
		bindingIdx++;
	}
	VkDescriptorSetLayout layout;
	VkHelper::vk_instance->create_descriptor_set_layout(layoutBindings, layout);
	layoutCache[hash] = layout;
	return layout;
}

VkDescriptorSet VkPool::acquire_descriptor_set(VkDescriptorSetLayout layout, std::size_t renderId, bool* cached)
{
	size_t swapchainIdx = VkHelper::vk_instance->get_current_swapchain_index();
	auto& cacheMap = descriptorSetCache[swapchainIdx];
	auto id = reinterpret_cast<std::uintptr_t>(layout);
	auto cacheRst = cacheMap.find(id);
	if (cacheRst == cacheMap.end())
	{
		cacheMap[id] = std::vector<VkDescriptorSetCacheWrapper>();
	}
	auto& arr = cacheMap[id];
	for (int i = 0; i < arr.size(); i++)
	{
		if (!arr[i].used)
		{
			arr[i].used = true;
			return arr[i].desc_set;
		}
	}
	VkDescriptorSetCacheWrapper wrapper;
	VkDescriptorSet descriptorSet;
	VkHelper::vk_instance->create_descriptor_sets(descriptorPool, descriptorSet, layout);
	wrapper.desc_set = descriptorSet;
	wrapper.used = true;
	arr.push_back(wrapper);
	return descriptorSet;
}

void VkPool::acquire_command_buffer(VkCommandBuffer* cb)
{
	auto swapchainIdx = VkHelper::vk_instance->get_current_swapchain_index();
	auto& cbInPool = commandBufferCache[swapchainIdx];
	if (cbInPool == nullptr)
	{
		VkHelper::vk_instance->create_cmd_buffer(cbInPool);
	}
	*cb = cbInPool;
}

void VkPool::acquire_render_group(VkPassGroup** rg)
{
	auto swapchainIdx = VkHelper::vk_instance->get_current_swapchain_index();
	renderGroupsCache[swapchainIdx] = new VkPassGroup();
	(*rg) = (renderGroupsCache[swapchainIdx]);
}
void VkPool::acquire_pipeline(VkRenderPass pass, VkShaderWrapper& Shader, VkPipeline* pipeline, bool trans, uint32_t msaa)//vk_shader, VkRenderPass, bool, uint32_t
{
	intptr_t ptrAddr = reinterpret_cast<intptr_t>(pass);
	auto id = ptrAddr + Shader.id << 8;
	auto swapchainIdx = VkHelper::vk_instance->get_current_swapchain_index();
	auto& cache = pipelineCache[swapchainIdx];
	auto rst = cache.find(id);
	if (rst != cache.end())
	{
		*pipeline = rst->second;
	}
	else
	{
		VkHelper::vk_instance->create_graphics_pipeline(&Shader, pass, trans, msaa, pipeline);
		cache[id] = *pipeline;
	}
}
VkBufferWrapper VkPool::acquire_buffer2(uint32_t size)
{
	for (auto& buffer : bufferCache)
	{
		if (!buffer.in_recording && buffer.size == size)
		{
			buffer.in_recording = true;
			return buffer;
		}
	}
	VkBufferWrapper buffer;
	buffer.size = size;
	buffer.in_recording = true;
	VkHelper::vk_instance->create_uniform_buffer2(size, buffer.buffer, buffer.mem);
	bufferCache.push_back(buffer);
	return buffer;
}
VkDescriptorPool VkPool::get_descriptor_pool()
{
	return  descriptorPool;
}
std::shared_ptr<VkPass> VkPool::acquire_render_pass(std::shared_ptr<VkFrameBufferWrapper> info)
{
	auto swapchainIdx = VkHelper::vk_instance->get_current_swapchain_index();
	auto& cache = renderPassCache[swapchainIdx];
	auto rst = std::make_shared<VkPass>(info);
	cache.push_back(rst);
	return rst;
}
void VkPool::cleanup()
{
	auto inst = VkHelper::vk_instance;
	inst->destroy_descriptor_pool(descriptorPool);
	for (size_t i = 0; i < swapchain_count; i++)
	{
		auto swapchainIdx = i;
		auto& pipelineSubCache = pipelineCache[swapchainIdx];
		for (auto& pair : pipelineSubCache)
		{
			inst->destroy_pipeline(pair.second);
		}
		pipelineSubCache.clear();
		if (renderGroupsCache[swapchainIdx] != nullptr)
		{
			delete renderGroupsCache[swapchainIdx];
			renderGroupsCache[swapchainIdx] = nullptr;
		}
		auto& cache = renderPassCache[swapchainIdx];
		cache.clear();
		commandBufferCache.clear();
		for (auto& buffer : bufferCache)
		{
			inst->destroy_buffer(buffer.buffer, buffer.mem);
		}
		bufferCache.clear();
		descriptorSetCache.clear();
	}
	for (auto layoutIt = layoutCache.begin(); layoutIt != layoutCache.end(); layoutIt++)
	{
		auto& destVec = layoutIt->second;
		inst->destroy_descriptor_set_layout(destVec);
	}
	inst->pool->camera_base_fb = nullptr;
}
void VkPool::refresh()
{
	auto swapchainIdx = VkHelper::vk_instance->get_current_swapchain_index();
	GfxTexture::set_current_swapchain_idx(swapchainIdx);
	auto& pipelineSubCache = pipelineCache[swapchainIdx];
	for (auto& pair : pipelineSubCache)
	{
		VkHelper::vk_instance->destroy_pipeline(pair.second);
	}
	pipelineSubCache.clear();
	if (renderGroupsCache[swapchainIdx] != nullptr)
	{
		delete renderGroupsCache[swapchainIdx];
		renderGroupsCache[swapchainIdx] = nullptr;
	}

	if (commandBufferCache[swapchainIdx] != nullptr)
		VkResult rst = vkResetCommandBuffer(commandBufferCache[swapchainIdx], VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	for (auto& buffer : bufferCache)
	{
		buffer.in_recording = false;
	}
	auto& cache = renderPassCache[swapchainIdx];
	cache.clear();
	auto& cacheMap = descriptorSetCache[swapchainIdx];
	for (auto& pair : cacheMap)
	{
		for (auto& descSet : pair.second)
		{
			descSet.used = false;
		}
	}
}
VkPool::VkPool(size_t swapchainCount)
{
	swapchain_count = swapchainCount;
	commandBufferCache.resize(swapchainCount);
	descriptorSetCache.resize(swapchainCount);
	renderGroupsCache.resize(swapchainCount);
	pipelineCache.resize(swapchainCount);
	renderPassCache.resize(swapchainCount);
	GfxTexture::init(swapchainCount);
	VkDescriptorPoolSize pool_sizes[] =
	{
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * 11;
	pool_info.poolSizeCount = 11;
	pool_info.pPoolSizes = pool_sizes;
	VkHelper::vk_instance->create_descriptor_pool(pool_info, descriptorPool);
}

_VOXEL_END
