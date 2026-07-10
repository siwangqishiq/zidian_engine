#pragma once

#include <vector>
#include "vulkan/vulkan.h"
#include "renderer/canvas.h"

namespace zidian{
    class Application;


    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback,
        void* user);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger);
    
    class Render : public ICanvas{
    public:
        Render(Application *appContext);

        void initVulkan(std::vector<const char *> &glfwExtenstinList);

        void onDispose();

        void beginRenderFrame();

        void endRenderFrame();

        void drawTriangles();
    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

        std::vector<const char *> instanceExtensions;
        std::vector<const char *> layers;

        void createInstance();

        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        Application *appCtx;
    };
}