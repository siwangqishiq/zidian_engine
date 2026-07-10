#include "renderer/render.h"
#include "utils/log.h"
#include "application.h"

namespace zidian {
    Render::Render(Application *appContext){
        appCtx = appContext;
    }

    void Render::initVulkan() {
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

        instance = VK_NULL_HANDLE;
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS){
            Log::e("render", "vkCreateInstance failed! error = %d", result);
            return;
        }
        Log::i("render", "Vulkan instance created successfully!");
    }

    void Render::beginRenderFrame(){
        // Log::i("render", "begin render frame");
    }

    void Render::endRenderFrame(){
        // Log::i("render", "end render frame");
    }

    void Render::onDispose(){
        if(instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }

    void Render::drawTriangles() {
        Log::i("render", "todo will draw triangles");
        
    }
}


