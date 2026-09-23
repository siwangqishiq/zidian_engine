#include "renderer/command/batch/simple_rect_batch.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"

namespace zidian{
    SimpleRectBatch::SimpleRectBatch(Render &ctx_) : Batch(ctx_){
        attachPipeline = std::make_unique<SimpleRectPipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();

        Log::green("pipeline", "create simple_rect pipeline success!");
        createBuffers();
    }

    void SimpleRectBatch::createBuffers(){
        vertexBuffers.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemorys.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemoryMappeds.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        offsets.resize(FrameResource::MAX_FRAME_IN_FLIGHT, 0);

        const uint32_t MAX_VERTEX_SIZE = 16 * 1024;

        for(uint32_t i = 0 ;i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            VkDeviceSize bufferSize = sizeof(SimpleRectVertex) * MAX_VERTEX_SIZE;

            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if(vkCreateBuffer(ctx.device, &bufferCreateInfo, nullptr, &vertexBuffers[i]) != VK_SUCCESS){
                Log::e("render", "Create simple rect batch vertex buffer failed!");
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
                Log::e("render", "allocte simple rect batch memory failed!");
                continue;
            }

            vkBindBufferMemory(ctx.device, vertexBuffers[i], vertexMemorys[i], 0);

            auto memoryMapResult = vkMapMemory(ctx.device, vertexMemorys[i], 0, bufferSize, 0, &vertexMemoryMappeds[i]);
            if(memoryMapResult != VK_SUCCESS){
                Log::e("render", "map the simple rect batch memory failed!");
                continue;
            }
        }//end for i
    }

    void SimpleRectBatch::init(){
    }

    bool SimpleRectBatch::canBatch(const Cmd& cmd){
        if(cmd.type != CmdType::DrawSimpleRect){
            return false;
        }
        return true;
    }

    void SimpleRectBatch::putCmd(const Cmd& cmd,uint32_t frameIndex){
        SimpleRectVertex item{};
        item.position = glm::vec3(cmd.rectData.left - cmd.rectData.width/2.0f, cmd.rectData.top - cmd.rectData.height/2.0f, 0.0f);
        item.size = glm::vec2(cmd.rectData.width, cmd.rectData.height);
        item.color = cmd.rectData.color;
        vertexData.emplace_back(item);
    }

    void SimpleRectBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex){
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);
        
        vertexData.clear();
    }

    void SimpleRectBatch::reset(){

    }

    SimpleRectBatch::~SimpleRectBatch(){
        for(int i = 0; i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            vkFreeMemory(ctx.device, vertexMemorys[i], nullptr);
            vkDestroyBuffer(ctx.device, vertexBuffers[i], nullptr);
        }//end for i
    }
}