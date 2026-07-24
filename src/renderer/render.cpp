#include <iostream>
#include <algorithm>
#include "renderer/render.h"
#include "utils/log.h"
#include "utils/format.h"
#include "application.h"
#include "config.h"
#include "renderer/vk_canvas.h"
#include "renderer/shader/shader_manager.h"

namespace zidian {
    Render::Render(Application &appContext):appCtx(appContext) {
    }

    Render::~Render(){
    }

    void Render::init(std::vector<const char *> &glfwExtenstinList){
        initVulkan(glfwExtenstinList);

        //init canvas
        canvas = std::make_unique<VkCanvas>(*this);
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
        createImageViews();
        createRenderPass();

        createPipelines();
        createCommandBuffers();
        createFramebuffers();
        createSyncObjects();

        memoryAllocator.init(physicalDevice, device); //初始化内存分配器
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
        appInfo.pApplicationName = appCtx.getAppConfig().name.c_str();
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
        surface = appCtx.createSurfaceFromInstance(instance);
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
       
        printMemoryInfo();
    }

    std::string Render::heapMemoryFlagsToStr(VkMemoryHeap &heap){
        std::string result = "";
        if(heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT){
            result = result + "device_local ";
        }

        if(heap.flags & VK_MEMORY_HEAP_MULTI_INSTANCE_BIT){
            result = result + "multi_instance ";
        }

        if(heap.flags & VK_MEMORY_HEAP_TILE_MEMORY_BIT_QCOM){
            result = result + "tile_memory_qcom ";
        }
        
        return result;
    }

    std::string Render::memoryPropertiesToStr(VkMemoryPropertyFlags &propFlags){
        std::string result = "";
        if(propFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT){
            result = result + "device_local ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT){
            result = result + "host_visible ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT){
            result = result + "host_coherent ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT){
            result = result + "host_cached ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT){
            result = result + "lazily_allocated ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_PROTECTED_BIT){
            result = result + "protected ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD){
            result = result + "device_coherent(amd) ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD){
            result = result + "device_uncached(amd) ";
        }
        if(propFlags & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV){
            result = result + "rdma_capable (nv) ";
        }
        return result;
    }

    void Render::printMemoryInfo(){
         VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        Log::green("render", "memoryTypeCount : %d", memProperties.memoryTypeCount);
        Log::green("render", "memoryHeapCount : %d", memProperties.memoryHeapCount);
        Log::green("render", "memory heap:");

        for(uint32_t i = 0 ; i < memProperties.memoryHeapCount ; i++){
            VkMemoryHeap heap = memProperties.memoryHeaps[i];
            Log::green("render", "heap type\theapsize : %s", Utils::FormatBytes(heap.size).c_str());
            Log::green("render", "heap type\tVkMemoryHeapFlags : %s", heapMemoryFlagsToStr(heap).c_str());
        }//end for i

        Log::green("render", "memory types:");
        for(uint32_t i = 0 ; i < memProperties.memoryTypeCount ; i++){
            VkMemoryType type = memProperties.memoryTypes[i];
            Log::green("render", "\t type -> %u     heap index -> %u", i, type.heapIndex);
            Log::green("render", "\t properties: %s",memoryPropertiesToStr(type.propertyFlags).c_str());
            Log::green("render", "\t==========================================");
        }//end for i
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

    void Render::createImageViews(){
        swapChainImageViews.resize(swapChainImages.size());
        for (int i = 0; i < swapChainImageViews.size(); i++) {
            VkImageViewCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.image = swapChainImages[i];
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = swapChainImageFormat;

            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            info.subresourceRange.baseMipLevel = 0;
            info.subresourceRange.levelCount = 1;
            info.subresourceRange.baseArrayLayer = 0;
            info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &info, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                Log::e("render", "create image views failed");
                throw std::runtime_error("create image view failed");
            }
        }//end for i
    }

    void Render::createRenderPass(){
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
            Log::e("render", "create render pass failed");
            throw std::runtime_error("create render pass failed");
        }
        Log::i("render", "create render pass success!");
    }

    void Render::createPipelines(){
        shaderManager = std::make_unique<ShaderManager>(*this);

        pipelines = std::make_unique<PipelineManager>(*this);
        pipelines->createPipelines();
    }

    void Render::createCommandBuffers(){
        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.queueFamilyIndex = graphQueueFamily;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult result = vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool);
        if(result != VK_SUCCESS){
            Log::e("render", "Create command pool failed!");
            return;
        }
        Log::i("render", "Create command pool success");
        

        commandBuffers.resize(MAX_FRAME_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = swapChainImages.size();

        result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
        if(result != VK_SUCCESS){
            Log::e("render", "Create command buffer failed!");
            return;
        }
        Log::i("render", "Create command buffer(count : %d) success", commandBuffers.size());
    }

    void Render::createFramebuffers(){
        frameBuffers.resize(swapChainImageViews.size());
        bool isFailed = false;
        for(int i = 0 ; i < swapChainImageViews.size() ; i++){
            VkFramebufferCreateInfo fbCreateInfo{};
            fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbCreateInfo.renderPass = renderPass;
            VkImageView attachments[] ={
                swapChainImageViews[i]
            };
            fbCreateInfo.attachmentCount = 1;
            fbCreateInfo.pAttachments = attachments;
            fbCreateInfo.layers = 1;
            fbCreateInfo.width = swapChainExtent.width;
            fbCreateInfo.height = swapChainExtent.height;

            if(vkCreateFramebuffer(device, &fbCreateInfo , nullptr, &frameBuffers[i]) != VK_SUCCESS){
                isFailed = true;
                Log::e("render", "Create framebuffer[%d] failed!", i);
                continue;
            }
        }//end for i

        if(!isFailed){
            return;
        }
        Log::i("render", "Create frame buffer success (%d)", frameBuffers.size());
    }

    void Render::createSyncObjects(){
        inFlightFences.resize(MAX_FRAME_IN_FLIGHT);
        imageAvailableSemaphores.resize(MAX_FRAME_IN_FLIGHT);

        for(uint32_t i = 0 ; i < MAX_FRAME_IN_FLIGHT; i++){
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS){
                Log::e("render", "Create imageAvailableSemaphore failed!");
                return;
            }

            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            if(vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS){
                Log::e("render", "Create inFightFence failed!");
                return;
            }
        }//end for i

        renderFinishSemaphores.resize(swapChainImageViews.size());
        for(uint32_t i = 0 ; i < swapChainImageViews.size(); i++){
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishSemaphores[i]) != VK_SUCCESS){
                Log::e("render", "Create renderFinishSemaphore failed!");
                return;
            }
        }//end for i
        
        Log::i("render", "Create sync object success imageAvailableSemaphore renderFinishSemaphore flightFence");
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
            if(appCtx.getAppConfig().vsync){ //开启垂直同步
                if(mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR){
                    return mode;
                }
                if(mode == VK_PRESENT_MODE_FIFO_KHR){
                    return mode;
                }
            }else { //关闭垂直同步
                if(mode == VK_PRESENT_MODE_IMMEDIATE_KHR){
                    return mode;
                }
                if(mode == VK_PRESENT_MODE_MAILBOX_KHR){
                    return mode;
                }
            }

            // if(mode == VK_PRESENT_MODE_MAILBOX_KHR){
            //     return mode;
            // }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Render::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
        int width = 0;
        int height = 0;
        appCtx.getFramebufferSize(width, height);
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

    std::unique_ptr<ICanvas>& Render::getCanvas(){
        return canvas;
    }

    bool Render::beginRenderFrame(){
        // Log::i("render", "begin render frame");
        //
        vkWaitForFences(device, 1, &inFlightFences[currentFrameIndex], VK_TRUE, UINT64_MAX);
        uint32_t imageIdx = UINT32_MAX;
        auto result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrameIndex], VK_NULL_HANDLE, &imageIdx);
        if(result != VK_SUCCESS && imageIdx != UINT32_MAX){
            Log::e("render" , "acquire image index failed.");
            return false;
        }

        currentImageIndex = imageIdx;

        vkResetFences(device, 1, &inFlightFences[currentFrameIndex]);

        //重置commandbuffer
        vkResetCommandBuffer(commandBuffers[currentFrameIndex], 0);

        //begin command buffer
        VkCommandBufferBeginInfo cfBeginInfo{};
        cfBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cfBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffers[currentFrameIndex], &cfBeginInfo);

        //begin render pass
        VkRenderPassBeginInfo passBeginInfo{};
        passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passBeginInfo.renderPass = renderPass;
        passBeginInfo.framebuffer = frameBuffers[currentImageIndex];
        VkRect2D rect = {
            {0,0},
            swapChainExtent
        };
        passBeginInfo.renderArea = rect;
        glm::vec4 configClearColor = appCtx.getAppConfig().clearColor;
        VkClearValue clearColor = {configClearColor[0],configClearColor[1],configClearColor[2],configClearColor[3]};
        passBeginInfo.clearValueCount = 1;
        passBeginInfo.pClearValues = &clearColor;
        vkCmdBeginRenderPass(commandBuffers[currentFrameIndex], &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        //清理命令列表
        commandList.reset();
        return true;
    }

    void Render::endRenderFrame(){
        uploadCommands();

        // Log::i("render", "end render frame");
        VkCommandBuffer& cmd = commandBuffers[currentFrameIndex];

        //end renderpass
        vkCmdEndRenderPass(cmd);

        //end command buffer
        vkEndCommandBuffer(cmd);

        //submit 
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] ={
            imageAvailableSemaphores[currentFrameIndex]
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        VkPipelineStageFlags waitStages[] ={
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;
        VkSemaphore signalSemaphores[] = {
            renderFinishSemaphores[currentImageIndex]
        };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        vkQueueSubmit(graphQueue, 1 , &submitInfo, inFlightFences[currentFrameIndex]);

        //present
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapchains[] = {
            swapChain
        };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &currentImageIndex;

        VkResult presentResult = vkQueuePresentKHR(graphQueue, &presentInfo);
        if(presentResult != VK_SUCCESS){
            Log::e("render", "present queue error!");
        }
        
        currentFrameIndex = (currentFrameIndex + 1) % MAX_FRAME_IN_FLIGHT;
        // Log::purple("render", "currentFrameIndex = %u", currentFrameIndex);
    }

    void Render::uploadCommands(){
        uploadPrimitive();
    }

    void Render::uploadPrimitive(){
        if(commandList.getPrimitiveCommands().empty()){
            return;
        }

        for(PrimitiveCommand &command : commandList.getPrimitiveCommands()){
            
        }//end for each
    }

    void Render::onDispose(){
        if (device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(device);
        }
        
        memoryAllocator.destroy();

        for(auto &fence : inFlightFences){
            vkDestroyFence(device, fence, nullptr);
        }
        inFlightFences.clear();

        for(auto &sema : imageAvailableSemaphores){
            vkDestroySemaphore(device, sema, nullptr);
        }
        imageAvailableSemaphores.clear();
        for(auto &sema : renderFinishSemaphores){
            vkDestroySemaphore(device, sema, nullptr);
        }
        renderFinishSemaphores.clear();

        for(auto &fb : frameBuffers){
            vkDestroyFramebuffer(device, fb, nullptr);
        }//end for each

        if(commandPool != VK_NULL_HANDLE){
            vkDestroyCommandPool(device, commandPool, nullptr);
        }

        pipelines->clearPipelines();

        if(renderPass != VK_NULL_HANDLE){
            vkDestroyRenderPass(device, renderPass, nullptr);
        }

        for (int i = 0; i < swapChainImageViews.size(); i++) {
            vkDestroyImageView(device, swapChainImageViews[i], nullptr);
        }//end for i

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

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback,
        void* user
    ) {
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
