#include "renderer/command/batch/sprite_batch.h"
#include "renderer/pipeline/pipe_sprite.h"
#include "renderer/command/data/data_sprite.h"
#include "utils/log.h"
#include <variant>

namespace zidian{
    SpriteBatch::SpriteBatch(Render &ctx_) : Batch(ctx_) {
        attachPipeline = std::make_unique<SpritePipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();
        Log::i("pipeline", "create SpriteBatch success!");
    }

    void SpriteBatch::init(){
        createBuffers();
        Log::i("pipeline", "create SpriteBatch buffer success!");
    }

    VkDeviceSize SpriteBatch::genBufferSize(){
        const uint32_t MAX_VERTEX_SIZE = 16 * 1024 * 1024;
        return sizeof(SpriteVertex) * MAX_VERTEX_SIZE;
    }

    bool SpriteBatch::canBatch(const Cmd& cmd) {
        if(cmd.type != CmdType::DrawSprite){
            return false;
        }

        return true;
    }
    
    void SpriteBatch::putCmd(const Cmd& cmd, uint32_t frameIndex) {
        // const DrawTrianglesData& trianglesData = std::get<DrawTrianglesData>(cmd.data);
        // for(int i = 0 ; i < trianglesData.vertexCount; i++){
        //     TriangleVertex item;
        //     item.position = glm::vec3(trianglesData.vertices[i], 1.0f);
        //     item.color = trianglesData.colors[i];

        //     // Log::purple("debug", "item.position %f %f  color %f %f %f %f ", item.position[0], item.position[1], item.color[0],item.color[1],item.color[2],item.color[3]);
        //     vertexData.emplace_back(item);
        // }//end for i
    }
    
    void SpriteBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) {
        // vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);

        // const uint32_t vertexCount = vertexData.size();
        // auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        // memcpy(dst, vertexData.data(), vertexCount * sizeof(TriangleVertex));
        // VkDeviceSize offset[] = {offsets[frameIndex]};

        // vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        // offsets[frameIndex] += vertexCount * sizeof(TriangleVertex);

        // pushConstantDatas[frameIndex].proj = {
        //     glm::vec4(2.0f / ctx.swapChainExtent.width, 0.0f, 0.0f, 0.0f),
        //     glm::vec4(0.0f, 2.0f / ctx.swapChainExtent.height, 0.0f, 0.0f),
        //     glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        //     glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f)
        // };
        // vkCmdPushConstants(cmdBuffer, attachPipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CommonUniform), &pushConstantDatas[frameIndex]);
        // // Log::green("pipeline", "commit triangles batch! vertexCount =  %d", vertexCount);
        // vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();
        ctx.drawCallCount++;
    }
    
    void SpriteBatch::reset(uint32_t frameIndex) {
        offsets[frameIndex] = 0;
    }

    VkDescriptorSetLayout SpriteBatch::getDescriptorSetLayout() {
        return attachPipeline->descriptorSetLayout;
    }
    
    SpriteBatch::~SpriteBatch(){
        // Log::purple("batch", "SpriteBatch destroy buffer");
    }
}