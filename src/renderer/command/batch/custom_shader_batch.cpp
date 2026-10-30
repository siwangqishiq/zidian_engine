#include "renderer/command/batch/custom_shader_batch.h"
#include "utils/log.h"
#include <variant>
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/pipe_custom_shader.h"

namespace zidian{
    CustomShaderBatch::CustomShaderBatch(Render &ctx_) : Batch(ctx_) {
        Log::i("pipeline", "create CustomShaderBatch success!");
    }

    void CustomShaderBatch::init(){
        createBuffers();
        Log::i("pipeline", "create CustomShaderBatch buffer success!");
    }

    VkDeviceSize CustomShaderBatch::genBufferSize(){
        const uint32_t MAX_VERTEX_SIZE = 8 * 1024;
        return sizeof(CustomShaderVertex) * MAX_VERTEX_SIZE;
    }

    bool CustomShaderBatch::canBatch(const Cmd& cmd) {
        if(cmd.type != CmdType::DrawCustomShader){
            return false;
        }

        const DrawCustomShaderData& customShaderData = std::get<DrawCustomShaderData>(cmd.data);
        if(customShaderData.pipeId == currentPipelineId){
            return true;
        }
        return false;
    }
    
    void CustomShaderBatch::putCmd(const Cmd& cmd, uint32_t frameIndex) {
        const DrawCustomShaderData& customShaderData = std::get<DrawCustomShaderData>(cmd.data);

        float posLeft = customShaderData.dstRect.left;
        float posRight = customShaderData.dstRect.right();
        float posTop = customShaderData.dstRect.top;
        float posBottom = customShaderData.dstRect.bottom();

        CustomShaderVertex vertex[4];
        // 0 1 2
        vertex[0].position = glm::vec3(posRight, posTop, 1.0f);
        vertex[0].uv = glm::vec2(1.0f, 0.0f);

        vertex[1].position = glm::vec3(posLeft, posTop, 1.0f);
        vertex[1].uv = glm::vec2(0.0f, 0.0f);

        vertex[2].position = glm::vec3(posLeft, posBottom, 1.0f);
        vertex[2].uv = glm::vec2(0.0f, 1.0f);

        vertex[3].position = glm::vec3(posRight, posBottom, 1.0f);
        vertex[3].uv = glm::vec2(1.0f, 1.0f);

        vertexData.emplace_back(vertex[0]);
        vertexData.emplace_back(vertex[1]);
        vertexData.emplace_back(vertex[2]);

        vertexData.emplace_back(vertex[0]);
        vertexData.emplace_back(vertex[2]);
        vertexData.emplace_back(vertex[3]);

        currentPipelineId = customShaderData.pipeId;
    }
    
    void CustomShaderBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) {
        if(currentPipelineId == -1){
            return;
        }

        const uint32_t vertexCount = vertexData.size();
        if(vertexCount == 0){
            return;
        }

        auto customPipeline = ctx.pipelineManager->findCustomPipeline(currentPipelineId);
        if(customPipeline == nullptr){
            return;
        }

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, customPipeline->pipeline);

        auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        memcpy(dst, vertexData.data(), vertexCount * sizeof(CustomShaderVertex));
        VkDeviceSize offset[] = {offsets[frameIndex]};

        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        offsets[frameIndex] += vertexCount * sizeof(CustomShaderVertex);
        updateTransformMatrix(cmdBuffer, frameIndex , customPipeline->pipelineLayout);

        vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();
        ctx.drawCallCount++;
    }
    
    void CustomShaderBatch::reset(uint32_t frameIndex) {
        offsets[frameIndex] = 0;
    }

    CustomShaderBatch::~CustomShaderBatch(){
    }
}