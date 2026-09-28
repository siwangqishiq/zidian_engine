#include "renderer/command/batch/simple_triangle_batch.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"

namespace zidian{
    SimpleTriangleBatch::SimpleTriangleBatch(Render &ctx_) : Batch(ctx_){
        attachPipeline = std::make_unique<SimpleTrianglePipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();

        Log::green("pipeline", "create simple_triangle pipeline success!");
        createBuffers();
    }

    void SimpleTriangleBatch::createBuffers(){
        vertexBuffers.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemorys.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        vertexMemoryMappeds.resize(FrameResource::MAX_FRAME_IN_FLIGHT);
        pushConstantDatas.resize(FrameResource::MAX_FRAME_IN_FLIGHT);

        offsets.resize(FrameResource::MAX_FRAME_IN_FLIGHT, 0);

        const uint32_t MAX_VERTEX_SIZE = 16 * 1024;

        for(uint32_t i = 0 ;i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            VkDeviceSize bufferSize = sizeof(SimpleTriangleVertex) * MAX_VERTEX_SIZE;

            VkBufferCreateInfo bufferCreateInfo{};
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = bufferSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if(vkCreateBuffer(ctx.device, &bufferCreateInfo, nullptr, &vertexBuffers[i]) != VK_SUCCESS){
                Log::e("render", "Create simple triangle batch vertex buffer failed!");
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
                Log::e("render", "allocte simple triangle batch memory failed!");
                continue;
            }

            vkBindBufferMemory(ctx.device, vertexBuffers[i], vertexMemorys[i], 0);

            auto memoryMapResult = vkMapMemory(ctx.device, vertexMemorys[i], 0, bufferSize, 0, &vertexMemoryMappeds[i]);
            if(memoryMapResult != VK_SUCCESS){
                Log::e("render", "map the simple triangle batch memory failed!");
                continue;
            }
        }//end for i
    }

    void SimpleTriangleBatch::init(){
    }

    bool SimpleTriangleBatch::canBatch(const Cmd& cmd){
        if(cmd.type != CmdType::DrawSimpleTriangle){
            return false;
        }

        return true;
    }

    void SimpleTriangleBatch::putCmd(const Cmd& cmd,uint32_t frameIndex){
        glm::vec2 p1 = cmd.triangleData.p1;
        glm::vec2 p2 = cmd.triangleData.p2;
        glm::vec2 p3 = cmd.triangleData.p3;
        auto aabb = geometry.buildAABB(p1, p2, p3);

        glm::vec2 leftTopPoint = aabb[0];
        glm::vec2 rightBottomPoint = aabb[1];
        glm::vec2 center = (leftTopPoint + rightBottomPoint) / 2.0f;

        float width = rightBottomPoint[0] - leftTopPoint[0];
        float height = rightBottomPoint[1] - leftTopPoint[1];
        float size = glm::max(width, height);

        SimpleTriangleVertex item{};
        item.position= glm::vec3(center, 0.0f);
        item.size = size;
        item.color = cmd.triangleData.color;

        glm::vec2 p1Local = p1 - leftTopPoint;
        glm::vec2 p2Local = p2 - leftTopPoint;
        glm::vec2 p3Local = p3 - leftTopPoint;

        glm::vec2 p1_{p1Local[0]/ width, p1Local[1]/height};
        glm::vec2 p2_{p2Local[0]/ width, p2Local[1]/height};
        glm::vec2 p3_{p3Local[0]/ width, p3Local[1]/height};

        item.p1 = p1_;
        item.p2 = p2_;
        item.p3 = p3_;

        // Log::green("debug", "size = %f",size);
        // Log::green("debug" , "p1(%f,%f) p2(%f, %f) p3(%f,%f)", 
        //             p1_[0],p1_[1], p2_[0],p2_[1] , p3_[0],p3_[1]);

        vertexData.emplace_back(item);
    }

    void SimpleTriangleBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex){
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);

        const uint32_t vertexCount = vertexData.size();
        auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        memcpy(dst, vertexData.data(), vertexCount * sizeof(SimpleTriangleVertex));
        VkDeviceSize offset[] = {offsets[frameIndex]};
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        offsets[frameIndex] += vertexCount * sizeof(SimpleTriangleVertex);

        pushConstantDatas[frameIndex].proj = {
            glm::vec4(2.0f / ctx.swapChainExtent.width, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, 2.0f / ctx.swapChainExtent.height, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f)
        };
        vkCmdPushConstants(cmdBuffer, attachPipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CommonUniform), &pushConstantDatas[frameIndex]);
        
        // Log::green("pipeline", "commit simple triangle batch!");
        vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();

        ctx.drawCallCount++;
    }

    void SimpleTriangleBatch::reset(uint32_t frameIndex){
        offsets[frameIndex] = 0;
    }

    SimpleTriangleBatch::~SimpleTriangleBatch(){
        for(int i = 0; i < FrameResource::MAX_FRAME_IN_FLIGHT; i++){
            vkFreeMemory(ctx.device, vertexMemorys[i], nullptr);
            vkDestroyBuffer(ctx.device, vertexBuffers[i], nullptr);
        }//end for i
    }
}