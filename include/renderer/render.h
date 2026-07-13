#pragma once

#include <vector>
#include <memory>
#include "vulkan/vulkan.h"
#include "renderer/canvas.h"
#include "renderer/pipeline/pipeline_manager.h"

namespace zidian{
    class Application;
    class PipelineManager;


    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback,
        void* user);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger);

    struct SwapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    class Render : public ICanvas{
    public:
        Render(Application *appContext);

        void initVulkan(std::vector<const char *> &glfwExtenstinList);

        void onDispose();

        void beginRenderFrame();

        void endRenderFrame();

        void drawTriangles(glm::vec2 *verts, int count);

        std::unique_ptr<PipelineManager> pipelines;
    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;

        uint32_t graphQueueFamily = UINT32_MAX;
        VkQueue graphQueue = VK_NULL_HANDLE;

        uint32_t presentFamily = UINT32_MAX;

        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        uint32_t swapChainImageCount = 0;
        std::vector<VkImageView> swapChainImageViews;

        std::vector<const char *> instanceExtensions;
        std::vector<const char *> layers;

        VkRenderPass renderPass = VK_NULL_HANDLE;

        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicDevice();
        void createSwapchain();
        void createImageViews();
        void createRenderPass();
        void createPipelines();

        bool isPhyDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceProperties props);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        uint32_t findGraphQueueFamily(VkPhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        Application *appCtx;
    };
}