#include "renderer/render.h"
#include "renderer/frame_resource.h"
#include "utils/log.h"

namespace zidian {
    FrameResource::FrameResource(Render &context) : ctx(context){
    }

    FrameResource::~FrameResource(){
    }

    void FrameResource::init(){
        createFramebuffers();
        createCommandBuffers();
        createSyncObjects();
        createPrimitiveVertexBuffers();
        createPushConstantDatas();
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

    void FrameResource::createPrimitiveVertexBuffers(){
        primitiveVertexBuffers.resize(MAX_FRAME_IN_FLIGHT);
        primitiveVertexMemorys.resize(MAX_FRAME_IN_FLIGHT);
        primitiveMemoryMappeds.resize(MAX_FRAME_IN_FLIGHT);

        const uint32_t MAX_VERTEX_SIZE = 1024 * 1024;

        for(uint32_t i = 0 ;i < MAX_FRAME_IN_FLIGHT; i++){
            primitiveVertexMaxCounts.push_back(MAX_VERTEX_SIZE);

            if(!createPrimitiveVertexBuffer(i)){
                Log::e("render", "createPrimitiveVertexBuffer failed! index : %u", i);
                break;
            }
        }//end for i
    }

    bool FrameResource::createPrimitiveVertexBuffer(const uint32_t index){
        if(index >= MAX_FRAME_IN_FLIGHT){
            return false;
        }

        VkDeviceSize bufferSize = sizeof(PrimitiveVertex) * primitiveVertexMaxCounts[index];

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if(vkCreateBuffer(ctx.device, &bufferCreateInfo, nullptr, &primitiveVertexBuffers[index]) != VK_SUCCESS){
            Log::e("render", "Create primitive vertex buffer failed!");
            return false;
        }

        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(ctx.device, primitiveVertexBuffers[index], &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = ctx.memoryAllocator.findMemoryType(memRequirements.memoryTypeBits, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        if(vkAllocateMemory(ctx.device,&allocInfo,nullptr,&primitiveVertexMemorys[index]) != VK_SUCCESS){
            Log::e("render", "allocte primitive memory failed!");
            return false;
        }

        vkBindBufferMemory(ctx.device, primitiveVertexBuffers[index], primitiveVertexMemorys[index], 0);

        auto memoryMapResult = vkMapMemory(ctx.device, primitiveVertexMemorys[index], 0, bufferSize, 0, &primitiveMemoryMappeds[index]);
        if(memoryMapResult != VK_SUCCESS){
            Log::e("render", "map the primitive memory failed!");
            return false;
        }
        
        return true;
    }

    void FrameResource::createPushConstantDatas(){
        for(uint32_t i = 0 ; i < MAX_FRAME_IN_FLIGHT; i++){
            pushConstDatas.push_back({
                glm::mat4()
            });
        }//end for i;
    }


    void FrameResource::destroy(){
        VkDevice &device = ctx.device;
        for(uint32_t i = 0 ; i < MAX_FRAME_IN_FLIGHT; i++){
            vkUnmapMemory(ctx.device, primitiveVertexMemorys[i]);
            vkDestroyBuffer(device, primitiveVertexBuffers[i], nullptr);
            vkFreeMemory(device, primitiveVertexMemorys[i], nullptr);
        }//end for i;

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
}


