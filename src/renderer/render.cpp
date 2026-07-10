#include <iostream>
#include <algorithm>
#include "renderer/render.h"
#include "utils/log.h"
#include "application.h"
#include "config.h"

namespace zidian {
    Render::Render(Application *appContext){
        appCtx = appContext;
    }

    void Render::initVulkan(std::vector<const char *> &glfwExtenstinList) {
        instanceExtensions.clear();
        instanceExtensions.insert(instanceExtensions.end(), glfwExtenstinList.begin(), glfwExtenstinList.end());
        if(IS_DEBUG){
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        Log::i("render", "Need set up extension:");
        for(auto &e : instanceExtensions){
            Log::i("render", "\t%s", e);
        }//end for each

        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicDevice();
        createSwapchain();
    }

    void Render::createInstance() {
        Log::i("render","render init vulkan");
        Log::i("render", "vulkan header version %d",VK_HEADER_VERSION);

        uint32_t instanceVersion;
        vkEnumerateInstanceVersion(&instanceVersion);

        Log::i("render", "vulkan instance version \n\tmarjor: %d  \n\tminor: %d  \n\tpatch: %d",
            VK_VERSION_MAJOR(instanceVersion),
            VK_VERSION_MINOR(instanceVersion), 
            VK_VERSION_PATCH(instanceVersion));

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appCtx->getAppConfig().name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "zidian";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = instanceExtensions.size();
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
        
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if(layerCount > 0){
            std::vector<VkLayerProperties> layers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount , layers.data());
            Log::i("render", "layers:");
            for(auto &p : layers){
                Log::i("render", "\t%s", p.layerName);
            }//end for each
        }else{
            Log::e("render", "Not found layers");
        }

        if(IS_DEBUG){
            layers.push_back("VK_LAYER_KHRONOS_validation");
            
            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.enabledLayerCount = layers.size();
            createInfo.ppEnabledLayerNames = layers.data();
            createInfo.pNext = &debugCreateInfo;
        }else{
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS){
            Log::e("render", "vkCreateInstance failed! error = %d", result);
            return;
        }
        
        Log::green("render", "Vulkan instance created successfully!");

        if(IS_DEBUG){
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            populateDebugMessengerCreateInfo(createInfo);
            if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) 
                != VK_SUCCESS){
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }
    }

    void Render::createSurface(){
        surface = appCtx->createSurfaceFromInstance(instance);
    }

    void Render::pickPhysicalDevice(){
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if(deviceCount == 0){
            Log::e("render", "No Vulkan physical device found");
            throw std::runtime_error("No Vulkan physical device found.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        Log::i("render", "Find %d physical devices:", deviceCount);

        for(VkPhysicalDevice &device : devices){
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);
            Log::i("render", "\t %s type: %d", props.deviceName, props.deviceType);
        }//end for each

        for(VkPhysicalDevice &device : devices){
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);
            
            if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
                physicalDevice = device;
                break;
            }
        }//end for each

        if(physicalDevice == VK_NULL_HANDLE){
            for(VkPhysicalDevice &device : devices){
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(device, &props);
                if(isPhyDeviceSuitable(device, props)){
                    physicalDevice = device;
                    break;
                }
            }//end for each
        }

        if(physicalDevice == VK_NULL_HANDLE){
            Log::e("render", "No suitable GPU found");
            throw std::runtime_error("No suitable GPU found");
        }else{
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevice, &props);
            Log::i("render", "Select GPU : %s", props.deviceName);
        }
    }

    bool Render::isPhyDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceProperties props){
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for(auto &queueFamily : queueFamilies){
            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
                return true;
            }
        }

        return false;
    }
    
    void Render::createLogicDevice(){
        graphQueueFamily = findGraphQueueFamily(physicalDevice);
        Log::i("render", "Graphics Queue Family : %d", graphQueueFamily);

        const float priority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = graphQueueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        const char* deviceExtensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 1;
        createInfo.ppEnabledExtensionNames = deviceExtensions;

        if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
            throw std::runtime_error("Create logical device failed.");
        }
        vkGetDeviceQueue(device, graphQueueFamily, 0, &graphQueue);

        Log::i("render", "Create logical device success.");
    }

    void Render::createSwapchain(){
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphQueueFamily, surface, &presentSupport);

        if(!presentSupport){
            throw std::runtime_error("graphQueueFamily not support present");
        }

        Log::i("render", "graphQueueFamily is supported present : %d", presentSupport);
        presentFamily = graphQueueFamily;
        
        SwapChainSupportDetails support = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
        VkExtent2D extent = chooseSwapExtent(support.capabilities);

        uint32_t imageCount = support.capabilities.minImageCount + 1;
        if(support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount){
            imageCount = support.capabilities.maxImageCount;
        }

        Log::i("render", "imageCount : %d", imageCount);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {graphQueueFamily,presentFamily};

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.preTransform = support.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS){
            throw std::runtime_error("create swapchain failed");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        
        vkGetSwapchainImagesKHR(device,swapChain,&imageCount,swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;

        Log::i("render", "create swapchain success image count = %d", imageCount);
    }

    SwapChainSupportDetails Render::querySwapChainSupport(VkPhysicalDevice device){
        SwapChainSupportDetails details{};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if(formatCount > 0){
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, nullptr);

        if(presentCount > 0){
            details.presentModes.resize(presentCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR Render::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats){
        for(const auto& format : formats){
            if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
                return format;
            }
        }

        return formats[0];
    }

    VkPresentModeKHR Render::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes){
        for(auto mode : modes){
            if(mode == VK_PRESENT_MODE_MAILBOX_KHR){
                return mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Render::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
        int width = 0;
        int height = 0;
        appCtx->getFramebufferSize(width, height);
        Log::i("render", "Get framebuffer size %d x %d", width, height);
        
        if(capabilities.currentExtent.width != UINT32_MAX){
            return capabilities.currentExtent;
        }

        VkExtent2D extent{};
        extent.width = std::clamp(
            static_cast<uint32_t>(width),
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);

        extent.height = std::clamp(
            static_cast<uint32_t>(height),
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);

        return extent;
    }

    uint32_t Render::findGraphQueueFamily(VkPhysicalDevice device){
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        Log::i("render", "Graphics Queue Family Count: %d", queueFamilyCount);
        for(uint32_t i = 0; i < queueFamilyCount; i++){
            if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                return i;
            }
        }

        throw std::runtime_error("Graphics queue not found.");
    }

    void Render::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT 
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    
        createInfo.pfnUserCallback = DebugCallback;
    }

    void Render::beginRenderFrame(){
        // Log::i("render", "begin render frame");
    }

    void Render::endRenderFrame(){
        // Log::i("render", "end render frame");
    }

    void Render::onDispose(){
        vkDestroySwapchainKHR(device, swapChain, nullptr);

        if(device != VK_NULL_HANDLE){
            vkDestroyDevice(device, nullptr);
        }

        if(surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }

        if(debugMessenger != VK_NULL_HANDLE){
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger);
        }

        if(instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }

    void Render::drawTriangles(glm::vec2 *verts, int count) {
        Log::i("render", "todo will draw triangles");
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback,
        void* user) {
        
        if(severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT){
            Log::e("Debug", "%s",callback->pMessage);
        }else{
            Log::w("Debug", "%s",callback->pMessage);
        }

        return VK_FALSE;
    }


    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger){
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, messenger, nullptr);
        }
    }
}
