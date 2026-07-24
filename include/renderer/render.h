#pragma once

#include <vector>
#include <memory>
#include "vulkan/vulkan.h"
#include "renderer/canvas.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/command/command_list.h"
#include "renderer/mem/memory_allocator.h"

namespace zidian{
    class Application;
    class PipelineManager;
    class ShaderManager;
    


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
    
    class Render {
    public:
        const uint32_t MAX_FRAME_IN_FLIGHT = 2;
        uint32_t currentFrameIndex = 0;

        Render(Application &appContext);

        void init(std::vector<const char *> &glfwExtenstinList);

        void onDispose();

        bool beginRenderFrame();

        void endRenderFrame();

        std::unique_ptr<ICanvas>& getCanvas();

        CommandList commandList;

        virtual ~Render();

        std::unique_ptr<PipelineManager> pipelines;
        std::unique_ptr<ShaderManager> shaderManager;

        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;

        VkQueue graphQueue = VK_NULL_HANDLE;
        
        uint32_t graphQueueFamily = UINT32_MAX;
        uint32_t presentFamily = UINT32_MAX;

        uint32_t currentImageIndex = UINT32_MAX;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        uint32_t swapChainImageCount = 0;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> frameBuffers;

        std::vector<const char*> instanceExtensions;
        std::vector<const char*> layers;

        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;

        //GPU同步对象
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishSemaphores;

        //CPU -> GPU 同步对象
        std::vector<VkFence> inFlightFences;

        MemoryAllocator memoryAllocator;
    private:
        void initVulkan(std::vector<const char *> &glfwExtenstinList);
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicDevice();
        void createSwapchain();
        void createImageViews();
        void createRenderPass();
        void createPipelines();
        void createCommandBuffers();
        void createFramebuffers();
        void createSyncObjects();

        void printMemoryInfo();

        void uploadCommands();
        void uploadPrimitive();
        

        bool isPhyDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceProperties props);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        uint32_t findGraphQueueFamily(VkPhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        std::string heapMemoryFlagsToStr(VkMemoryHeap &heap);
        std::string memoryPropertiesToStr(VkMemoryPropertyFlags &propFlags);

        Application &appCtx;
        std::unique_ptr<ICanvas> canvas;
    };
}