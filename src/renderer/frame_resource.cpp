#include "renderer/render.h"
#include "renderer/frame_resource.h"
#include "utils/log.h"
#include "renderer/pipeline/primitive_uniform_data.h"


namespace zidian {
    uint32_t FrameResource::MAX_FRAME_IN_FLIGHT = 2;

    FrameResource::FrameResource(Render &context) : ctx(context){
    }

    FrameResource::~FrameResource(){
    }

    void FrameResource::init(){
        createFramebuffers();
        createCommandBuffers();
        createSyncObjects();
        createPushConstantDatas();

        createTransformBuffers();
        createGlobalDescriptorSetLayout();
        createGlobalDescriptorSetPool();
        

        //创建完buffer后  更新descriptorSet
        ctx.pipelineManager->updateDescriptorSets();
        // createUniformDatas();
    }

    void FrameResource::createFramebuffers(){
        frameBuffers.resize(ctx.swapChainImageViews.size());
        bool isFailed = false;
        for(int i = 0 ; i < ctx.swapChainImageViews.size() ; i++){
            VkFramebufferCreateInfo fbCreateInfo{};
            fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbCreateInfo.renderPass = ctx.renderPass;
            VkImageView attachments[] ={
                ctx.swapChainImageViews[i]
            };
            fbCreateInfo.attachmentCount = 1;
            fbCreateInfo.pAttachments = attachments;
            fbCreateInfo.layers = 1;
            fbCreateInfo.width = ctx.swapChainExtent.width;
            fbCreateInfo.height = ctx.swapChainExtent.height;

            if(vkCreateFramebuffer(ctx.device, &fbCreateInfo , nullptr, &frameBuffers[i]) != VK_SUCCESS){
                isFailed = true;
                Log::e("render", "Create framebuffer[%d] failed!", i);
                continue;
            }
        }//end for i

        if(isFailed){
            return;
        }
        Log::i("render", "Create frame buffer success (%d)", frameBuffers.size());
    }

    void FrameResource::createCommandBuffers() {
        commandBuffers.resize(MAX_FRAME_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = ctx.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = MAX_FRAME_IN_FLIGHT;

        auto result = vkAllocateCommandBuffers(ctx.device, &allocInfo, commandBuffers.data());
        if(result != VK_SUCCESS){
            Log::e("render", "Create command buffer failed!");
            return;
        }
        Log::i("render", "Create command buffer(count : %d) success", commandBuffers.size());
    }

    void FrameResource::createSyncObjects(){
        inFlightFences.resize(MAX_FRAME_IN_FLIGHT);
        imageAvailableSemaphores.resize(MAX_FRAME_IN_FLIGHT);

        for(uint32_t i = 0 ; i < MAX_FRAME_IN_FLIGHT; i++){
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS){
                Log::e("render", "Create imageAvailableSemaphore failed!");
                return;
            }

            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            if(vkCreateFence(ctx.device, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS){
                Log::e("render", "Create inFightFence failed!");
                return;
            }
        }//end for i

        renderFinishSemaphores.resize(ctx.swapChainImageViews.size());
        for(uint32_t i = 0 ; i < ctx.swapChainImageViews.size(); i++){
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if(vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &renderFinishSemaphores[i]) != VK_SUCCESS){
                Log::e("render", "Create renderFinishSemaphore failed!");
                return;
            }
        }//end for i
        
        Log::i("render", "Create sync object success imageAvailableSemaphore renderFinishSemaphore flightFence");
    }

    void FrameResource::createPushConstantDatas(){
        for(uint32_t i = 0 ; i < MAX_FRAME_IN_FLIGHT; i++){
            pushConstDatas.push_back({
                glm::mat4()
            });
        }//end for i;
    }

    void FrameResource::createTransformBuffers(){
        transformBuffers.resize(MAX_FRAME_IN_FLIGHT);
        transformMemorys.resize(MAX_FRAME_IN_FLIGHT);
        transformMemoryMappeds.resize(MAX_FRAME_IN_FLIGHT);

        transformDatas.resize(MAX_FRAME_IN_FLIGHT);

        constexpr VkDeviceSize bufferSize = sizeof(glm::mat4) * MAX_TRANSFORM_COUNT;
        for (uint32_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++){
            //创建 Storage Buffer
            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(ctx.device,&bufferCreateInfo,nullptr,&transformBuffers[i]) != VK_SUCCESS) {
                Log::e("render", "Create transform storage buffer failed!");
                continue;
            }

            //分配 device memory
            VkMemoryRequirements memRequirements{};
            vkGetBufferMemoryRequirements(ctx.device,transformBuffers[i],&memRequirements);
            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = ctx.memoryAllocator.findMemoryType(
                memRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            );

            if (vkAllocateMemory(ctx.device,&allocInfo,nullptr,&transformMemorys[i]) != VK_SUCCESS) {
                Log::e("render", "Allocate transform storage memory failed!");
                continue;
            }

            if (vkBindBufferMemory(ctx.device,transformBuffers[i],transformMemorys[i],0) != VK_SUCCESS) {
                Log::e("render", "Bind transform storage buffer memory failed!");
                continue;
            }

            // map memory
            if (vkMapMemory(ctx.device,transformMemorys[i],0,bufferSize,0,&transformMemoryMappeds[i]) != VK_SUCCESS) {
                Log::e("render", "Map transform storage memory failed!");
                continue;
            }

            transformDatas[i].reserve(MAX_TRANSFORM_COUNT);
        }//end for i
    }

    //创建DescriptotrSetLayout
    void FrameResource::createGlobalDescriptorSetLayout(){
        VkDescriptorSetLayoutBinding bindings[1]{};
        bindings[0].binding = 0;
        bindings[0].descriptorCount = 1;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = 1;
        layoutCreateInfo.pBindings = bindings;

        if(vkCreateDescriptorSetLayout(ctx.device, &layoutCreateInfo, nullptr, &globalDescSetLayout_) != VK_SUCCESS){
            Log::e("render", "create global descriptorset layout error");
            return;
        }

        Log::i("render", "create global descriptorset layout success!");
    }

    //创建DescriptotrSetPool
    void FrameResource::createGlobalDescriptorSetPool(){
        
    }
    
    //创建DescriptotrSet 并完成更新
    void FrameResource::allocAndUpdateGlobalDescriptorSet(){

    }

    void FrameResource::uploadTransformData(const uint32_t frameIndex){
        auto &transforms = transformDatas[frameIndex];
        if (transforms.empty()) {
            return;
        }

        if (transforms.size() > MAX_TRANSFORM_COUNT) {
            Log::e("render", "Transform count exceeds maximum capacity!");
            return;
        }

        memcpy(transformMemoryMappeds[frameIndex],transforms.data(),transforms.size() * sizeof(glm::mat4));
    }

    void FrameResource::destroyTransformBuffers(){
        for (uint32_t i = 0; i < transformBuffers.size(); i++) {
            if (transformMemoryMappeds[i]) {
                vkUnmapMemory(ctx.device, transformMemorys[i]);
                transformMemoryMappeds[i] = nullptr;
            }

            if (transformBuffers[i] != VK_NULL_HANDLE) {
                vkDestroyBuffer(ctx.device, transformBuffers[i], nullptr);
                transformBuffers[i] = VK_NULL_HANDLE;
            }

            if (transformMemorys[i] != VK_NULL_HANDLE) {
                vkFreeMemory(ctx.device, transformMemorys[i], nullptr);
                transformMemorys[i] = VK_NULL_HANDLE;
            }
        }
    }

    void FrameResource::destroy(){
        VkDevice &device = ctx.device;

        destroyGlobalDescriptorSetResources();
        destroyTransformBuffers();

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

        destroyFramebuffers();
    }

    void FrameResource::destroyFramebuffers(){
        for(auto &fb : frameBuffers){
            vkDestroyFramebuffer(ctx.device, fb, nullptr);
        }//end for each
        frameBuffers.clear();
    }

    VkDescriptorSetLayout& FrameResource::getGlobalDescriptorSetLayout(){
        return globalDescSetLayout_;
    }

    void FrameResource::destroyGlobalDescriptorSetResources(){
        if(globalDescSetLayout_ != VK_NULL_HANDLE){
            vkDestroyDescriptorSetLayout(ctx.device, globalDescSetLayout_, nullptr);
        }
    }
}


