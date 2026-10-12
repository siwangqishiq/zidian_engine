#include "renderer/command/batch/simple_circle_batch.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"
#include <variant>

namespace zidian{
    SimpleCircleBatch::SimpleCircleBatch(Render &ctx_) : Batch(ctx_){
        attachPipeline = std::make_unique<SimpleCirclePipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();

        Log::green("pipeline", "create simple_circle pipeline success!");
    }

    VkDeviceSize SimpleCircleBatch::genBufferSize() {
        const uint32_t MAX_VERTEX_SIZE = 2 * 1024 * 1024;
        return sizeof(SimpleCircleVertex) * MAX_VERTEX_SIZE;
    }

    void SimpleCircleBatch::init(){
        createBuffers();
        Log::green("pipeline", "create simple_circle buffer success~");
    }

    bool SimpleCircleBatch::canBatch(const Cmd& cmd){
        if(cmd.type != CmdType::DrawSimpleCircle){
            return false;
        }
        return true;
    }

    void SimpleCircleBatch::putCmd(const Cmd& cmd,uint32_t frameIndex){
        SimpleCircleVertex item{};
        const DrawSimpleCircleData &data = std::get<DrawSimpleCircleData>(cmd.data);
        item.position = glm::vec3(data.center, 0.0f);
        item.radius = data.radius;
        item.color = data.color;

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
        Log::purple("batch", "SimpleCircleBatch destroy buffer");
    }
}