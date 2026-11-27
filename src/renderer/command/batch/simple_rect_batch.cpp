#include "renderer/command/batch/simple_rect_batch.h"
#include "utils/log.h"
#include "renderer/frame_resource.h"
#include <variant>

namespace zidian{
    SimpleRectBatch::SimpleRectBatch(Render &ctx_) : Batch(ctx_){
        attachPipeline = std::make_unique<SimpleRectPipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();

        Log::i("pipeline", "create simple_rect pipeline success!");
    }

    VkDeviceSize SimpleRectBatch::genBufferSize() {
        const uint32_t MAX_VERTEX_SIZE = 1024 * 1024;
        VkDeviceSize bufferSize = sizeof(SimpleRectVertex) * MAX_VERTEX_SIZE;
        return bufferSize;
    }

    void SimpleRectBatch::init(){
        createBuffers();
        Log::i("pipeline", "create simple_rect buffer success~");
    }

    bool SimpleRectBatch::canBatch(const Cmd& cmd){
        if(cmd.type != CmdType::DrawSimpleRect){
            return false;
        }
        return true;
    }

    void SimpleRectBatch::putCmd(const Cmd& cmd,uint32_t frameIndex){
        SimpleRectVertex item{};
        const DrawSimpleRectData &data = std::get<DrawSimpleRectData>(cmd.data);
        float size = data.width / 2.0f;
        item.position = glm::vec3(data.left + size, data.top + size, 0.0f);
        item.size = glm::vec2(data.width, data.height);
        item.color = data.color;
        item.matIndex = data.transformMatrixIndex;

        vertexData.emplace_back(item);
    }

    void SimpleRectBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex){
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);

        const uint32_t vertexCount = vertexData.size();
        auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        memcpy(dst, vertexData.data(), vertexCount * sizeof(SimpleRectVertex));
        VkDeviceSize offset[] = {offsets[frameIndex]};
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        offsets[frameIndex] += vertexCount * sizeof(SimpleRectVertex);

        updatePushConstants(cmdBuffer, frameIndex , attachPipeline->pipelineLayout);

        vkCmdBindDescriptorSets(
            cmdBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            attachPipeline->pipelineLayout,
            0,
            1,
            &ctx.frameResource->globalDescriptorSets_[frameIndex],
            0,
            nullptr
        );
        
        // Log::green("pipeline", "commit simple rect batch!");
        vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();
        
        ctx.drawCallCount++;
    }

    void SimpleRectBatch::reset(uint32_t frameIndex){
        offsets[frameIndex] = 0;
    }

    SimpleRectBatch::~SimpleRectBatch(){
        // Log::purple("batch", "SimpleRectBatch destroy buffer");
    }
}