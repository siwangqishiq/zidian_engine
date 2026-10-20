#include "renderer/command/batch/triangle_batch.h"
#include "renderer/pipeline/pipe_triangle.h"
#include "renderer/command/data/data_triangles.h"
#include "utils/log.h"
#include <variant>

namespace zidian{
    TriangleBatch::TriangleBatch(Render &ctx_) : Batch(ctx_) {
        attachPipeline = std::make_unique<TrianglePipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();
        Log::i("pipeline", "create TrianglePipeline success!");
    }

    void TriangleBatch::init(){
        createBuffers();
        Log::i("pipeline", "create Triangle buffer success!");
    }

    VkDeviceSize TriangleBatch::genBufferSize(){
        const uint32_t MAX_VERTEX_SIZE = 2 * 1024 * 1024;
        return sizeof(TriangleVertex) * MAX_VERTEX_SIZE;
    }

    bool TriangleBatch::canBatch(const Cmd& cmd) {
        if(cmd.type != CmdType::DrawTriangles){
            return false;
        }
        return true;
    }
    
    void TriangleBatch::putCmd(const Cmd& cmd, uint32_t frameIndex) {
        const DrawTrianglesData& trianglesData = std::get<DrawTrianglesData>(cmd.data);
        for(int i = 0 ; i < trianglesData.vertexCount; i++){
            TriangleVertex item;
            item.position = glm::vec3(trianglesData.vertices[i], 1.0f);
            item.color = trianglesData.colors[i];

            // Log::purple("debug", "item.position %f %f  color %f %f %f %f ", item.position[0], item.position[1], item.color[0],item.color[1],item.color[2],item.color[3]);
            vertexData.emplace_back(item);
        }//end for i
    }
    
    void TriangleBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) {
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);

        const uint32_t vertexCount = vertexData.size();
        auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        memcpy(dst, vertexData.data(), vertexCount * sizeof(TriangleVertex));
        VkDeviceSize offset[] = {offsets[frameIndex]};

        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        offsets[frameIndex] += vertexCount * sizeof(TriangleVertex);

        // pushConstantDatas[frameIndex].proj = {
        //     glm::vec4(2.0f / ctx.swapChainExtent.width, 0.0f, 0.0f, 0.0f),
        //     glm::vec4(0.0f, 2.0f / ctx.swapChainExtent.height, 0.0f, 0.0f),
        //     glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        //     glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f)
        // };
        // vkCmdPushConstants(cmdBuffer, attachPipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CommonUniform), &pushConstantDatas[frameIndex]);
        // Log::green("pipeline", "commit triangles batch! vertexCount =  %d", vertexCount);
        updateTransformMatrix(cmdBuffer, frameIndex , attachPipeline->pipelineLayout);

        vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();
        ctx.drawCallCount++;
    }
    
    void TriangleBatch::reset(uint32_t frameIndex) {
        offsets[frameIndex] = 0;
    }
    
    TriangleBatch::~TriangleBatch(){
        // Log::purple("batch", "TriangleBatch destroy buffer");
    }
}