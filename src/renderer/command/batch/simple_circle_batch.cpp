#include "renderer/command/batch/simple_circle_batch.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"

namespace zidian{
    SimpleCircleBatch::SimpleCircleBatch(Render &ctx_) : Batch(ctx_){
        attachPipeline = std::make_unique<SimpleCirclePipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();

        Log::green("pipeline", "create simple_circle pipeline success!");
        createBuffers();
        Log::green("pipeline", "create simple_circle buffer success~");
    }

    void SimpleCircleBatch::createBuffers(){
        vertexBuffers.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemorys.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemoryMappeds.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        pushConstantDatas.resize(FrameResource::MAX_FRAME_IN_FLIGHT);

        offsets.resize(FrameResource::MAX_FRAME_IN_FLIGHT, 0);

        const uint32_t MAX_VERTEX_SIZE = 16 * 1024;

        for(uint32_t i = 0 ;i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            VkDeviceSize bufferSize = sizeof(SimpleCircleVertex) * MAX_VERTEX_SIZE;

            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if(vkCreateBuffer(ctx.device, &bufferCreateInfo, nullptr, &vertexBuffers[i]) != VK_SUCCESS){
                Log::e("render", "Create simple circle batch vertex buffer failed!");
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
                Log::e("render", "allocte simple circle batch memory failed!");
                continue;
            }

            vkBindBufferMemory(ctx.device, vertexBuffers[i], vertexMemorys[i], 0);

            auto memoryMapResult = vkMapMemory(ctx.device, vertexMemorys[i], 0, bufferSize, 0, &vertexMemoryMappeds[i]);
            if(memoryMapResult != VK_SUCCESS){
                Log::e("render", "map the simple circle batch memory failed!");
                continue;
            }
        }//end for i
    }

    void SimpleCircleBatch::init(){
    }

    bool SimpleCircleBatch::canBatch(const Cmd& cmd){
        if(cmd.type != CmdType::DrawSimpleCircle){
            return false;
        }
        return true;
    }

    void SimpleCircleBatch::putCmd(const Cmd& cmd,uint32_t frameIndex){
        SimpleCircleVertex item{};
        item.position = glm::vec3(cmd.circleData.center, 0.0f);
        item.radius = cmd.circleData.radius;
        item.color = cmd.circleData.color;
        vertexData.emplace_back(item);
    }

    void SimpleCircleBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex){
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);

        const uint32_t vertexCount = vertexData.size();
        auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        memcpy(dst, vertexData.data(), vertexCount * sizeof(SimpleCircleVertex));
        VkDeviceSize offset[] = {offsets[frameIndex]};
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        offsets[frameIndex] += vertexCount * sizeof(SimpleCircleVertex);

        pushConstantDatas[frameIndex].proj = {
            glm::vec4(2.0f / ctx.swapChainExtent.width, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 2.0f / ctx.swapChainExtent.height, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f)
        };
        vkCmdPushConstants(cmdBuffer, attachPipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CommonUniform), &pushConstantDatas[frameIndex]);
        
        vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();

        ctx.drawCallCount++;
    }

    void SimpleCircleBatch::reset(uint32_t frameIndex){
        offsets[frameIndex] = 0;
    }

    SimpleCircleBatch::~SimpleCircleBatch(){
        for(int i = 0; i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            vkFreeMemory(ctx.device, vertexMemorys[i], nullptr);
            vkDestroyBuffer(ctx.device, vertexBuffers[i], nullptr);
        }//end for i
    }
}