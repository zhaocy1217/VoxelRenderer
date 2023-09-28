#include "VkHelper.h"
#include <algorithm>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_glfw.h>
_VOXEL_BEGIN
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation",
	"VK_LAYER_RENDERDOC_Capture",
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_LUNARG_device_simulation",
	//"VK_LAYER_LUNARG_gfxreconstruct",
	//"VK_LAYER_LUNARG_monitor",
	//"VK_LAYER_LUNARG_screenshot",
};
const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME }; // VK_EXT_DEBUG_MARKER_EXTENSION_NAME
bool check_validation_layer_support()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (const auto& layerProperties : availableLayers)
	{
		std::cout << layerProperties.layerName << std::endl;
	}
	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}
std::vector<const char*> get_required_extensions(std::function<const char** (uint32_t*)>& get_ext_func, bool enable_validation_layers)
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = get_ext_func(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enable_validation_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}
VkResult static create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
};
void static destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debug_messenger, pAllocator);
	}
};
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	//std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
};
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debug_callback;
}
QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = i;
		}
		if (indices.isComplete())
		{
			break;
		}
		i++;
	}
	return indices;
}
SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}
bool check_device_extension_support(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());
	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}
bool is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices = find_queue_families(device, surface);
	bool extensionsSupported = check_device_extension_support(device);
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = query_swapchain_support(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}
VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}
VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
VkFormat find_supported_format(VkPhysicalDevice& physical_device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}
VkFormat find_depth_format(VkPhysicalDevice& physicsdevoce)
{
	return find_supported_format(physicsdevoce,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height) };
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
	}
}
VkHelper* VkHelper::vk_instance = nullptr;
void VkHelper::create_cmd_buffer(VkCommandBuffer& command_buffer)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	if (vkAllocateCommandBuffers(device, &allocInfo, &command_buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}
VkCommandBuffer VkHelper::create_cmd_buffer(std::vector<std::shared_ptr<VkRenderer>>& opaque_renderer, PostProcessing* pp, std::function<void()> gui)
{
	GfxTexture::clear_temp_cache();
	auto pool = VkHelper::vk_instance->pool;
	pool->refresh();
	VkCommandBuffer vkCmd;
	VkHelper::vk_instance->pool->acquire_command_buffer(&vkCmd);
	if (vkCmd == nullptr)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(vkCmd, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkPassGroup* rg;
	VkHelper::vk_instance->pool->acquire_render_group(&rg);
	{
		auto sourceTex = VkTextureWrapper::get_vktexture_from_gfx_tex(rg->passes[0]->fbo->color.get());
		VkImageMemBarrier barrier;
		barrier.old_layout = sourceTex->cur_layout;
		barrier.new_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.src_access_mask = VK_ACCESS_NONE;
		barrier.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkHelper::vk_instance->transit_layout(vkCmd, sourceTex, barrier);
	}
	rg->passes[0]->begin(vkCmd, opaque_renderer, true, { 0,0,0,0 }, true, 1);
	rg->passes[0]->end(vkCmd);


	if (pp != nullptr)
	{
		RenderContext render_context;
		RenderContext::set_blit_func([&](std::shared_ptr<GfxTexture> _source, std::shared_ptr<GfxTexture> _dest, std::shared_ptr<Material> _mat)
			{VkPassGroup::blit(vkCmd, _source, _dest, _mat); },
			[&](std::shared_ptr<GfxTexture> _source, std::shared_ptr<GfxTexture> _dest)
			{VkPassGroup::blit2(vkCmd, _source, _dest); });
		TextureBaseInfo base_info;
		base_info = rg->passes[0]->fbo->color->base_info;
		auto src = GfxTexture::acquire_temp_texture(base_info);
		VkPassGroup::blit2(vkCmd, rg->passes[0]->fbo->color, src);
		render_context.source = src;
		render_context.dest = rg->passes[0]->fbo->color;
		pp->on_render_post(render_context);
	}
	if (gui != nullptr)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		gui();
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		auto fb = std::make_shared<VkFrameBufferWrapper>(rg->passes[0]->fbo->color, nullptr);
		if (gui_render_pass->fbo != nullptr && gui_render_pass->fbo->vk_fbo != nullptr)
		{
			destroy_frame_buffer_object(gui_render_pass->fbo->vk_fbo);
		}
		gui_render_pass->fbo = fb;
		rebind_framebuffer_with_pass(gui_render_pass->renderPass, fb);
		auto _ = std::vector<std::shared_ptr<VkRenderer>>();
		gui_render_pass->begin(vkCmd, _, false, { 0,0,0,0 }, true, 1);
		ImGui_ImplVulkan_RenderDrawData(draw_data, vkCmd);
		gui_render_pass->end(vkCmd);

	}
	auto cur_swapchain = swapchain_textures[acquired_swapchain_idx];// swapchain_textures[get_current_swapchain_index()];
	rg->blit(vkCmd, rg->passes[0]->fbo->color, cur_swapchain);
	{
		VkImageMemBarrier barrier;
		auto tex = VkTextureWrapper::get_vktexture_from_gfx_tex(cur_swapchain.get());
		barrier.old_layout = tex->cur_layout;
		barrier.new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		barrier.src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		barrier.dst_access_mask = VK_ACCESS_NONE;
		VkHelper::vk_instance->transit_layout(vkCmd, tex, barrier);
	}
	if (vkEndCommandBuffer(vkCmd) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
	return vkCmd;
}

void VkHelper::start_vk(VkStartInfo& start_info)
{
	auto helper = new VoxelRendering::VkHelper();
	VoxelRendering::VkHelper::vk_instance = helper;
	helper->create_surface_func = start_info.create_surface_func;
	helper->get_ext_func = start_info.get_ext_func;
	helper->get_size_func = start_info.get_size_func;
	(*start_info.draw_func) = [&](std::vector<GfxRenderDataset*>& renderers, PostProcessing* pp, std::function<void()> gui) {
		VoxelRendering::VkHelper::vk_instance->render(renderers, pp, gui); };
	*start_info.clean_func = [&]() {VoxelRendering::VkHelper::vk_instance->cleanup(); };
	*start_info.idle_func = [&]() {VoxelRendering::VkHelper::vk_instance->wait_device_idle(); };
	start_info.set_raw_data_gen_tex_gfx_func([](unsigned char* data, TextureBaseInfo& info)
		{
			return (void*)(new VkTextureWrapper(data, info));
		},
		[](void* data)
		{
			delete (VkTextureWrapper*)data;
		});
	start_info.set_gen_shader_gfx_func([](std::vector<char>& v, std::vector<char>& f,
		ShaderVertexInput& vertexInput,
		std::vector<ShaderBindingObject>& resources_bindings) {
			auto vk_s = new VkShaderWrapper(v, f, vertexInput, resources_bindings);
			return  (void*)vk_s;
		},
		[](void* data)
		{
			delete (VkShaderWrapper*)data;
		});
	start_info.set_gen_mesh_gfx_func([](std::vector<float>& data, std::vector<unsigned int>& indices) {return (void*)(new VkMeshWrapper(data, indices)); },
		[](void* data)
		{
			delete (VkMeshWrapper*)data;
		});
	start_info.set_gen_tex_gfx_func([](TextureBaseInfo& info)
		{
			return (void*)(new VkTextureWrapper(info));
		},
		[](void* data)
		{
			delete (VkTextureWrapper*)data;
		}
		);
	helper->init();
	helper->initImGui(start_info.setup_imgui_window_func);
};
void VkHelper::render(std::vector<GfxRenderDataset*>& renderers, PostProcessing* pp, std::function<void()> gui)
{
	vkWaitForFences(device, 1, &in_flight_fences[current_swapchain_idx], VK_TRUE, UINT64_MAX);
	VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_swapchain_idx], VK_NULL_HANDLE, &acquired_swapchain_idx);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//  recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	if (images_in_flight[acquired_swapchain_idx] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device, 1, &images_in_flight[acquired_swapchain_idx], VK_TRUE, UINT64_MAX);
	}
	images_in_flight[acquired_swapchain_idx] = in_flight_fences[current_swapchain_idx];
	std::vector<std::shared_ptr<VkRenderer>> vk_renderers(renderers.size());
	for (size_t i = 0; i < renderers.size(); i++)
	{
		GfxRenderDataset* ptr = renderers[i];
		auto vkr = std::make_shared<VkRenderer>(ptr);
		vk_renderers[i] = vkr;
	}
	auto vkCmd = create_cmd_buffer(vk_renderers, pp, gui);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { image_available_semaphores[current_swapchain_idx] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vkCmd;
	VkSemaphore signalSemaphores[] = { render_finished_semaphores[current_swapchain_idx] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	vkResetFences(device, 1, &in_flight_fences[current_swapchain_idx]);
	if (vkQueueSubmit(graphics_queue, 1, &submitInfo, in_flight_fences[current_swapchain_idx]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &acquired_swapchain_idx;
	result = vkQueuePresentKHR(present_queue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		//    framebufferResized = false;
		//     recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
	current_swapchain_idx = (current_swapchain_idx + 1) % MAX_FRAMES_IN_FLIGHT;
}
void VkHelper::create_sync_objects()
{
	image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
	in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
	images_in_flight.resize(swapchain_textures.size(), VK_NULL_HANDLE);
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &image_available_semaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &render_finished_semaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &in_flight_fences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}
void VkHelper::wait_device_idle()
{
	vkDeviceWaitIdle(device);
}
void VkHelper::cleanup()
{
	delete gui_render_pass;
	if (enable_validation_layers)
		destroy_debug_utils_messenger_ext(instance, debug_messenger, nullptr);
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	vkDestroyCommandPool(device, command_pool, nullptr);
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	pool->cleanup();
	for (auto& swapchain : swapchain_textures)
	{
		auto tex = (VkTextureWrapper*)swapchain->handler;
		vkDestroyImageView(device, tex->image_view, nullptr);
	}
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> in_flight_fences;
	std::vector<VkFence> images_in_flight;
	for (auto& se : image_available_semaphores)
	{
		vkDestroySemaphore(device, se, nullptr);
	}
	for (auto& se : render_finished_semaphores)
	{
		vkDestroySemaphore(device, se, nullptr);
	}
	for (auto& se : in_flight_fences)
	{
		vkDestroyFence(device, se, nullptr);
	}
	for (auto& se : images_in_flight)
	{
		vkDestroyFence(device, se, nullptr);
	}
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}
void VkHelper::init()
{
	current_swapchain_idx = 0;
	if (enable_validation_layers && !check_validation_layer_support())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	auto extensions = get_required_extensions(get_ext_func, enable_validation_layers);
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (enable_validation_layers)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
	// create debug messager
	if (enable_validation_layers)
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);
		if (create_debug_utils_messenger_ext(instance, &createInfo, nullptr, &debug_messenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}
	// create surface
	/*if (glfwCreateWindowSurface(instance, m_window->glfwWindow, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}*/
	create_surface_func(instance, &surface);
	// create physics device
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices)
	{
		if (is_device_suitable(device, surface))
		{
			physical_device = device;
			//     msaaSamples = getMaxUsableSampleCount();
			break;
		}
	}
	if (physical_device == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	// create logic device
	QueueFamilyIndices indices = find_queue_families(physical_device, surface);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = device_extensions.data();
	if (enable_validation_layers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount = 0;
	}
	if (vkCreateDevice(physical_device, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &present_queue);
	uint32_t width;
	uint32_t height;
	get_size_func(&width, &height);
	SwapChainSupportDetails swapChainSupport = query_swapchain_support(physical_device, surface);
	VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats);
	VkPresentModeKHR presentMode = choose_swap_present_mode(swapChainSupport.presentModes);
	VkExtent2D extent = choose_swap_extent(swapChainSupport.capabilities, width, height);
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	// QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily)
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	std::vector<VkImage> swapChainImages;
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages.data());
	swapchain_image_format = surfaceFormat.format;
	swapChain_extent = extent;
	depth_format = find_depth_format(physical_device);
	std::vector<VkImageView> swapChainImageViews;
	swapChainImageViews.resize(swapChainImages.size());
	swapchain_textures.resize(imageCount);
	for (uint32_t i = 0; i < swapChainImages.size(); i++)
	{
		create_image_view(swapChainImages[i], swapChainImageViews[i], VkImageViewType::VK_IMAGE_VIEW_TYPE_2D, swapchain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		auto swtex = new VkTextureWrapper();
		swtex->image = swapChainImages[i];
		swtex->image_view = swapChainImageViews[i];
		swtex->format = swapchain_image_format;
		swtex->size = glm::uvec2(extent.width, extent.height);
		TextureBaseInfo info;
		info.size = swtex->size;
		info.srgb = true;
		info.channels = 4;
		info.cubemap = false;
		info.depth = false;
		info.mipLevels = 1;
		info.msaa = 1;
		auto gfxtex = std::make_shared<GfxTexture>(info, swtex, true);
		swapchain_textures[i] = gfxtex;
	}
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	if (vkCreateCommandPool(device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics command pool!");
	}
	auto swapchainCount = get_swapchain_count();
	create_sync_objects();
	pool = new VkPool(MAX_FRAMES_IN_FLIGHT);
	allocator_callback = new VkAllocationCallbacks();
	allocator_callback->pfnAllocation = ProfilingAllocationCallbacks::allocation;
	allocator_callback->pfnFree = ProfilingAllocationCallbacks::free_mem;
	allocator_callback->pfnReallocation = ProfilingAllocationCallbacks::reallocation;
}
void VkHelper::create_image_view(VkImage& image, VkImageView& image_view, VkImageViewType imageViewType, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = imageViewType;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = imageViewType == VK_IMAGE_VIEW_TYPE_CUBE ? 6 : 1;

	if (vkCreateImageView(device, &viewInfo, allocator_callback, &image_view) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}
}
uint32_t findMemoryType(VkPhysicalDevice& physical_device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}
void VkHelper::transition_image_layout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, int layerCount)
{
	VkCommandBuffer command_buffer = begin_single_time_commands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}
	vkCmdPipelineBarrier(
		command_buffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
	end_single_time_commands(command_buffer);
}

void VkHelper::copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, std::vector<VkBufferImageCopy>& bufferCopyRegions)
{
	VkCommandBuffer command_buffer = begin_single_time_commands();
	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()),
		bufferCopyRegions.data());
	end_single_time_commands(command_buffer);
}
void VkHelper::generate_mipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, int layerCount)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physical_device, imageFormat, &formatProperties);
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("texture image format does not support linear blitting!");
	}
	VkCommandBuffer command_buffer = begin_single_time_commands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;
	barrier.subresourceRange.levelCount = 1;
	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;
	for (uint32_t i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = layerCount;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = layerCount;
		vkCmdBlitImage(command_buffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
		if (mipWidth > 1)
			mipWidth /= 2;
		if (mipHeight > 1)
			mipHeight /= 2;
	}
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(command_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
	end_single_time_commands(command_buffer);
}
void VkHelper::initImGui(std::function<void()> setup_imgui_func)
{
	VkDescriptorPool imguiPool = pool->get_descriptor_pool();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	setup_imgui_func();
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = physical_device;
	init_info.Device = device;
	init_info.Queue = graphics_queue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = get_swapchain_count();
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.ColorAttachmentFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
	PassCreationInfo pass_info;
	pass_info.colorFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
	pass_info.msaa = 1;
	pass_info.flags = (uint32_t)ATTACHMENT_TYPE::COLOR;
	pass_info.colorOp = { VK_ATTACHMENT_LOAD_OP_DONT_CARE , VK_ATTACHMENT_STORE_OP_STORE };
	gui_render_pass = new VkPass();
	uint32_t h, w;
	get_window_size(&w, &h);
	gui_render_pass->size = glm::uvec2(w, h);
	create_render_pass(gui_render_pass->renderPass, pass_info);
	ImGui_ImplVulkan_Init(&init_info, gui_render_pass->renderPass);
	VkCommandBuffer command_buffer = begin_single_time_commands();
	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	end_single_time_commands(command_buffer);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}
void VkHelper::create_image(VkImage& textureImage, VkDeviceMemory& textureImageMemory, unsigned char* pixels, unsigned int texWidth, unsigned int texHeight, unsigned int channel,
	unsigned int mipLevels, bool cubemap, VkFormat format)
{
	int arrayLayer = cubemap ? 6 : 1;
	VkDeviceSize imageSize = texWidth * texHeight * channel * arrayLayer;
	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);
	create_image(texWidth, texHeight, mipLevels,
		VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		textureImage, textureImageMemory, arrayLayer);
	transition_image_layout(textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, arrayLayer);
	std::vector<VkBufferImageCopy> regions;
	if (!cubemap)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			texWidth,
			texHeight,
			1 };
		regions.push_back(region);
	}
	else {
		int layerSize = texWidth * texHeight * channel;
		for (size_t i = 0; i < 6; i++)
		{
			VkBufferImageCopy region{};
			region.bufferOffset = layerSize * i;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = i;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = { 0, 0, 0 };
			region.imageExtent = {
				texWidth,
				texHeight,
				1 };
			regions.push_back(region);
		}
	}
	copy_buffer_to_image(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), regions);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
	generate_mipmaps(textureImage, format, texWidth, texHeight, mipLevels, arrayLayer);
}
void VkHelper::create_image(uint32_t width, uint32_t height, uint32_t mipLevels,
	VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory, int arrayLayers)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	if (arrayLayers > 1)
	{
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	}
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = arrayLayers;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = numSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateImage(device, &imageInfo, allocator_callback, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physical_device, memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(device, &allocInfo, allocator_callback, &image_memory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}
	vkBindImageMemory(device, image, image_memory, 0);
}
void VkHelper::rebind_framebuffer_with_pass(VkRenderPass renderPass, std::shared_ptr<VkFrameBufferWrapper> fb)
{
	std::vector<VkImageView> attachments;
	auto color = VkTextureWrapper::get_vktexture_from_gfx_tex(fb->color.get());
	VkTextureWrapper* depth = nullptr;
	if (fb->depth != nullptr)
		depth = VkTextureWrapper::get_vktexture_from_gfx_tex(fb->depth.get());
	if (color != nullptr)
	{
		attachments.push_back(color->image_view);
	}
	if (depth != nullptr)
	{
		attachments.push_back(depth->image_view);
	}
	if (fb->additional_imageview != nullptr)
	{
		attachments.push_back(fb->additional_imageview);
	}
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = fb->size.x;
	framebufferInfo.height = fb->size.y;
	framebufferInfo.layers = 1;
	if (vkCreateFramebuffer(device, &framebufferInfo, allocator_callback, &(fb->vk_fbo)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
}
std::shared_ptr<VkFrameBufferWrapper> VkHelper::create_frame_buffer(glm::uvec2 size, VkFormat colorFormat, VkFormat depth_format, uint32_t msaaSamples, bool srgb,
	VkRenderPass& renderPass, VkImageView& additional_imageview)
{
	TextureBaseInfo info;
	info.size = size;
	info.channels = 4;
	info.mipLevels = 1;
	info.srgb = srgb;
	info.cubemap = false;
	info.msaa = 1;
	info.isAttachment = true;
	auto fbo = std::make_shared<VkFrameBufferWrapper>(info);
	auto color = VkTextureWrapper::get_vktexture_from_gfx_tex(fbo->color.get());
	auto depth = VkTextureWrapper::get_vktexture_from_gfx_tex(fbo->depth.get());
	std::vector<VkImageView> attachments;
	attachments.push_back(color->image_view);
	attachments.push_back(depth->image_view);
	if (additional_imageview != nullptr)
	{
		attachments.push_back(additional_imageview);
	}
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = size.x;
	framebufferInfo.height = size.y;
	framebufferInfo.layers = 1;
	if (vkCreateFramebuffer(device, &framebufferInfo, allocator_callback, &(fbo->vk_fbo)) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
	return fbo;
}

void VkHelper::create_texture_sampler(uint32_t mipLevels, VkSampler& textureSampler)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physical_device, &properties);
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(mipLevels);
	samplerInfo.mipLodBias = 0.0f;
	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}
VkCommandBuffer VkHelper::begin_single_time_commands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = command_pool;
	allocInfo.commandBufferCount = 1;
	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(device, &allocInfo, &command_buffer);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(command_buffer, &beginInfo);
	return command_buffer;
}
void VkHelper::end_single_time_commands(VkCommandBuffer command_buffer)
{
	vkEndCommandBuffer(command_buffer);
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_buffer;
	vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphics_queue);
	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}
void VkHelper::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer command_buffer = begin_single_time_commands();
	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, 1, &copyRegion);
	end_single_time_commands(command_buffer);
}
void VkHelper::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physical_device, memRequirements.memoryTypeBits, properties);
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
void VkHelper::create_vertex_buffer(std::vector<float>& vertices, VkBuffer& vertex_buffer, VkDeviceMemory& vertexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);
	create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer, vertexBufferMemory);
	copy_buffer(stagingBuffer, vertex_buffer, bufferSize);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}
void VkHelper::create_index_buffer(std::vector<unsigned int>& indices, VkBuffer& index_buffer, VkDeviceMemory& indexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);
	create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer, indexBufferMemory);
	copy_buffer(stagingBuffer, index_buffer, bufferSize);
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}
void VkHelper::create_shader_module(const std::vector<char>& code, VkShaderModule& shaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
}
void VkHelper::create_render_pass(VkRenderPass& rp, PassCreationInfo& creationInfo)
{
	bool hasColor = (creationInfo.flags & (uint32_t)ATTACHMENT_TYPE::COLOR) != 0;
	bool hasDepth = (creationInfo.flags & (uint32_t)ATTACHMENT_TYPE::DEPTH) != 0;
	bool hasResolve = creationInfo.msaa > 1;
	std::vector<VkAttachmentDescription>attachments = {};
	VkAttachmentDescription colorAttachment{};
	VkSubpassDescription subpass{};
	VkAttachmentReference colorAttachmentRef{};
	VkAttachmentReference depthAttachmentRef{};
	VkAttachmentReference colorAttachmentResolveRef{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	if (hasColor)
	{
		colorAttachment.format = creationInfo.colorFormat;
		colorAttachment.samples = (VkSampleCountFlagBits)creationInfo.msaa;
		colorAttachment.loadOp = creationInfo.colorOp.loadOp;/// VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = creationInfo.colorOp.storeOp;///VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments.push_back(colorAttachment);

		colorAttachmentRef.attachment = attachments.size() - 1;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pColorAttachments = &colorAttachmentRef;
	}
	VkAttachmentDescription depthAttachment{};
	if (hasDepth)
	{
		depthAttachment.format = depth_format;
		depthAttachment.samples = (VkSampleCountFlagBits)creationInfo.msaa;
		depthAttachment.loadOp = creationInfo.depthOp.loadOp;// VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = creationInfo.depthOp.storeOp; //VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments.push_back(depthAttachment);

		depthAttachmentRef.attachment = attachments.size() - 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
	}

	if (hasResolve)
	{
		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapchain_image_format;
		colorAttachmentResolve.samples = (VkSampleCountFlagBits)creationInfo.msaa;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachments.push_back(colorAttachmentResolve);

		colorAttachmentResolveRef.attachment = attachments.size() - 1;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;
	}
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if (vkCreateRenderPass(device, &renderPassInfo, allocator_callback, &rp) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
};
void VkHelper::create_pipeline_layout(VkDescriptorSetLayout layout, VkPipelineLayout* pipeline_layout)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &(layout);
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, pipeline_layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}
void VkHelper::create_graphics_pipeline(VkShaderWrapper* Shader, VkRenderPass& pass, bool isTransparent, uint32_t msaa, VkPipeline* pipeline)
{
	VkShaderModule& vert_shader_module = Shader->vert_shader_module; // .vertModule;//createShaderModule(vertShaderCode);
	VkShaderModule& frag_shader_module = Shader->frag_shader_module; // createShaderModule(fragShaderCode);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vert_shader_module;
	vertShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = frag_shader_module;
	fragShaderStageInfo.pName = "main";
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(Shader->attr_desc.size());
	vertexInputInfo.pVertexBindingDescriptions = &(Shader->binding_desc);
	vertexInputInfo.pVertexAttributeDescriptions = Shader->attr_desc.data();
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChain_extent.width;
	viewport.height = (float)swapChain_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain_extent;
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = (VkSampleCountFlagBits)msaa;
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	if (isTransparent)
	{
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}
	else
	{
		colorBlendAttachment.blendEnable = VK_FALSE;
	}
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = Shader->pipeline_layout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, allocator_callback, pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}
void VkHelper::create_descriptor_set_layout(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayout& descriptorSetLayout)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}
void VkHelper::create_descriptor_pool(VkDescriptorPoolCreateInfo& poolInfo, VkDescriptorPool& descriptorPool)
{
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
void VkHelper::create_descriptor_sets(VkDescriptorPool& descriptorPool, VkDescriptorSet& descriptorSet, VkDescriptorSetLayout& layout)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;
	if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
}
void VkHelper::update_descriptor_sets(
	uint32_t descriptorWriteCount,
	const VkWriteDescriptorSet* pDescriptorWrites,
	uint32_t descriptorCopyCount,
	const VkCopyDescriptorSet* pDescriptorCopies)
{
	vkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
}
void VkHelper::transit_layout(VkCommandBuffer vkCmd, VkTextureWrapper* tex, VkImageMemBarrier& vk_barrier)
{
	bool hasVkCmd = vkCmd != nullptr;
	if (!hasVkCmd)
		vkCmd = begin_single_time_commands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = vk_barrier.old_layout;
	barrier.newLayout = vk_barrier.new_layout;;
	barrier.image = tex->image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;
	barrier.srcAccessMask = vk_barrier.src_access_mask;
	barrier.dstAccessMask = vk_barrier.dst_access_mask;
	sourceStage = vk_barrier.src_stage_mask;
	destinationStage = vk_barrier.dst_stage_mask;
	vkCmdPipelineBarrier(
		vkCmd,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
	if (!hasVkCmd)
		end_single_time_commands(vkCmd);
	tex->cur_layout = barrier.newLayout;
};
void VkHelper::destroy_image(VkImage image, VkImageView view, VkSampler sampler, VkDeviceMemory mem)//(VkImage image, VkDeviceMemory& mem)
{
	vkDestroyImage(device, image, allocator_callback);
	vkDestroyImageView(device, view, allocator_callback);
	vkFreeMemory(device, mem, allocator_callback);
	if (sampler != nullptr)
	{
		vkDestroySampler(device, sampler, nullptr);
		//delete sampler;
	}
}
void VkHelper::destroy_buffer(VkBuffer buffer, VkDeviceMemory mem)
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, mem, nullptr);
}
void VkHelper::destroy_render_pass(VkRenderPass rp)
{
	vkDestroyRenderPass(device, rp, allocator_callback);
}
void VkHelper::destroy_pipeline_layout(VkPipelineLayout layout)
{
	vkDestroyPipelineLayout(device, layout, nullptr);
	//delete layout;
}
void VkHelper::destroy_shader_module(VkShaderModule layout)
{
	vkDestroyShaderModule(device, layout, nullptr);
}
void VkHelper::destroy_descriptor_set_layout(VkDescriptorSetLayout layout)
{
	vkDestroyDescriptorSetLayout(device, layout, nullptr);
}
void VkHelper::destroy_descriptor_pool(VkDescriptorPool pool)
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}
void VkHelper::destroy_pipeline(VkPipeline pipe)
{
	vkDestroyPipeline(device, pipe, allocator_callback);
}
void VkHelper::destroy_frame_buffer_object(VkFramebuffer fbo)
{
	vkDestroyFramebuffer(device, fbo, allocator_callback);
}

void VkHelper::get_window_size(uint32_t* w, uint32_t* h)
{
	get_size_func(w, h);
}
uint32_t VkHelper::get_swapchain_count()
{
	return swapchain_textures.size();
}
VkFormat VkHelper::get_swapchain_image_format() {
	return swapchain_image_format;
}
uint32_t VkHelper::get_current_swapchain_index()
{
	return current_swapchain_idx;
}
VkFormat VkHelper::get_depth_format() {
	return depth_format;
}
_VOXEL_END
