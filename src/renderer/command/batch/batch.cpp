#include "renderer/command/batch/batch.h"
#include "utils/log.h"

namespace zidian{
    Batch::Batch(Render &ctx_) : ctx(ctx_){
    }

    void Batch::init(){
        // createBuffers();
    }

    void Batch::createBuffers(){
        vertexBuffers.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemorys.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemoryMappeds.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        pushConstantDatas.resize(FrameResource::MAX_FRAME_IN_FLIGHT);

        offsets.resize(FrameResource::MAX_FRAME_IN_FLIGHT, 0);

        for(uint32_t i = 0 ;i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            VkDeviceSize bufferSize = genBufferSize();

            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if(vkCreateBuffer(ctx.device, &bufferCreateInfo, nullptr, &vertexBuffers[i]) != VK_SUCCESS){
                Log::e("batch", "Create simple circle batch vertex buffer failed!");
                continue;
            }

            VkMemoryRequirements memRequirements{};
            vkGetBufferMemoryRequirements(ctx.device, vertexBuffers[i], &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = ctx.memoryAllocator.findMemoryType(memRequirements.memoryTypeBits, 
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            
            if(vkAllocateMemory(ctx.device,&allocInfo,nullptr,&vertexMemorys[i]) != VK_SUCCESS){
                Log::e("batch", "allocte simple circle batch memory failed!");
                continue;
            }

            vkBindBufferMemory(ctx.device, vertexBuffers[i], vertexMemorys[i], 0);

            auto memoryMapResult = vkMapMemory(ctx.device, vertexMemorys[i], 0, bufferSize, 0, &vertexMemoryMappeds[i]);
            if(memoryMapResult != VK_SUCCESS){
                Log::e("batch", "map the simple circle batch memory failed!");
                continue;
            }
        }//end for i
    }

    Batch::~Batch(){
        dispose();
    }

    void Batch::dispose(){
        for(int i = 0; i < vertexBuffers.size(); i++){
            vkDestroyBuffer(ctx.device, vertexBuffers[i], nullptr);
        }
        vertexBuffers.clear();

        for(int i = 0; i < vertexMemorys.size(); i++){
            vkFreeMemory(ctx.device, vertexMemorys[i], nullptr);
        }
        vertexMemorys.clear();
    }
}


