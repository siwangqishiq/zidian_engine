#include <iostream>
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
        if(debugMessenger != VK_NULL_HANDLE){
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger);
        }

        if(instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }

    void Render::drawTriangles() {
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
