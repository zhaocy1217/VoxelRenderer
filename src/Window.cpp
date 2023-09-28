#include "window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include<backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
_VOXEL_BEGIN
void framebufferResizeCallback(GLFWwindow* w, int width, int height)
{
}
Window::Window()
{
    init();
}
void Window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto w = glfwCreateWindow(WIDTH, HEIGHT, "Voxel", nullptr, nullptr);
    glfwSetWindowUserPointer(w, this);
    glfwSetFramebufferSizeCallback(w, framebufferResizeCallback);
    glfwSetWindowAttrib(w, GLFW_RESIZABLE, false);
    glfwSetWindowAttrib(w, GLFW_FLOATING, false);
    windowHandler = w;
}
void Window::run()
{
    GLFWwindow* glfwWindow = (GLFWwindow*)windowHandler;
    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();
        if(m_drawFrameFunc!=nullptr)
            m_drawFrameFunc();
    }
    if (m_waitDeviceIdleFunc != nullptr)
        m_waitDeviceIdleFunc();
    if (m_cleanFunc != nullptr)
        m_cleanFunc();
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}
void Window::create_surface(VkInstance instance, VkSurfaceKHR* surface)
{
    GLFWwindow* glfwWindow = (GLFWwindow*)windowHandler;
    if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, surface) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}
void Window::setup_imgui()
{
    GLFWwindow* glfwWindow = (GLFWwindow*)windowHandler;
    ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
}
void Window::get_size(uint32_t* w, uint32_t* h)
{
    GLFWwindow* glfwWindow = (GLFWwindow*)windowHandler;
    int _w;
    int _h;
    glfwGetWindowSize(glfwWindow, &_w, &_h);
    *w = _w;
    *h = _h;
}
const char** Window::get_ext(uint32_t* count)
{
    return glfwGetRequiredInstanceExtensions(count);
}
Window::~Window()
{
}

_VOXEL_END