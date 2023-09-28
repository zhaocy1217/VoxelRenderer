#pragma once
#include "Defines.h"
#include "vulkan/vulkan_core.h"
_VOXEL_BEGIN
typedef void (*callback_function)(void*);
class Window
{
private:
	void init();
public:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
	callback_function resize_callback;
	std::function<void()> m_cleanFunc;
	std::function<void()> m_drawFrameFunc;
	std::function<void()> m_waitDeviceIdleFunc;
	std::function<void()> on_close;
	void* windowHandler;
	Window();
	void run();
	void get_size(uint32_t* w, uint32_t* h);

	const char** get_ext(uint32_t* count);
	void create_surface(VkInstance instance, VkSurfaceKHR* surface);
	void setup_imgui();
	~Window();
};
_VOXEL_END
