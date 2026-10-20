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
        const uint32_t MAX_VERTEX_SIZE = 32 * 1024 * 1024;
        return sizeof(SpriteVertex) * MAX_VERTEX_SIZE;
    }

    bool SpriteBatch::canBatch(const Cmd& cmd) {
        if(cmd.type != CmdType::DrawSprite){
            return false;
        }

        // Log::brown("debug", "put cmd canbatch");
        if(currentImage == nullptr){
            return true;
        }

        const DrawSpriteData& spriteData = std::get<DrawSpriteData>(cmd.data);
        if(spriteData.image != currentImage){
            return false;
        }
        return true;
    }
    
    void SpriteBatch::putCmd(const Cmd& cmd, uint32_t frameIndex) {
        // Log::brown("debug", "put cmd sprite");

        const DrawSpriteData& spriteData = std::get<DrawSpriteData>(cmd.data);

        if(spriteData.image == nullptr){
            return;
        }

        float uvLeft = (spriteData.srcRect.left) / static_cast<float>(spriteData.image->width);
        float uvTop = (spriteData.srcRect.top) / static_cast<float>(spriteData.image->height);
        float uvRight = (spriteData.srcRect.right()) / static_cast<float>(spriteData.image->width);
        float uvBottom = (spriteData.srcRect.bottom()) / static_cast<float>(spriteData.image->height);

        float posLeft = spriteData.dstRect.left;
        float posRight = spriteData.dstRect.right();
        float posTop = spriteData.dstRect.top;
        float posBottom = spriteData.dstRect.bottom();

        SpriteVertex vertex[4];
        // 0 1 2
        vertex[0].position = glm::vec3(posRight, posTop, 1.0f);
        vertex[0].uv = glm::vec2(uvRight, uvTop);
        vertex[0].color = spriteData.color;

        vertex[1].position = glm::vec3(posLeft, posTop, 1.0f);
        vertex[1].uv = glm::vec2(uvLeft, uvTop);
        vertex[1].color = spriteData.color;

        vertex[2].position = glm::vec3(posLeft, posBottom, 1.0f);
        vertex[2].uv = glm::vec2(uvLeft, uvBottom);
        vertex[2].color = spriteData.color;

        vertex[3].position = glm::vec3(posRight, posBottom, 1.0f);
        vertex[3].uv = glm::vec2(uvRight, uvBottom);
        vertex[3].color = spriteData.color;

        vertexData.emplace_back(vertex[0]);
        vertexData.emplace_back(vertex[1]);
        vertexData.emplace_back(vertex[2]);

        vertexData.emplace_back(vertex[0]);
        vertexData.emplace_back(vertex[2]);
        vertexData.emplace_back(vertex[3]);

        currentImage = spriteData.image;
    }
    
    void SpriteBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) {
        const uint32_t vertexCount = vertexData.size();
        if(vertexCount == 0){
            return;
        }

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipeline);
        auto* dst = static_cast<std::byte*>(vertexMemoryMappeds[frameIndex]) + offsets[frameIndex];
        memcpy(dst, vertexData.data(), vertexCount * sizeof(SpriteVertex));
        VkDeviceSize offset[] = {offsets[frameIndex]};

        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffers[frameIndex], offset);
        offsets[frameIndex] += vertexCount * sizeof(SpriteVertex);

        // pushConstantDatas[frameIndex].proj = {
        //     glm::vec4(2.0f / ctx.swapChainExtent.width, 0.0f, 0.0f, 0.0f),
        //     glm::vec4(0.0f, 2.0f / ctx.swapChainExtent.height, 0.0f, 0.0f),
        //     glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        //     glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f)
        // };
        // vkCmdPushConstants(cmdBuffer, attachPipeline->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CommonUniform), &pushConstantDatas[frameIndex]);

        updateTransformMatrix(cmdBuffer, frameIndex , attachPipeline->pipelineLayout);

        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, attachPipeline->pipelineLayout, 0, 
                1, &currentImage->descriptorSets[frameIndex], 0, nullptr);

        vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);

        vertexData.clear();
        ctx.drawCallCount++;
        // Log::purple("debug", "drawCallCount = %d vertexCount = %u", ctx.drawCallCount ,vertexCount);
    }
    
    void SpriteBatch::reset(uint32_t frameIndex) {
        offsets[frameIndex] = 0;
    }

    VkDescriptorSetLayout SpriteBatch::getDescriptorSetLayout() {
        return attachPipeline->descriptorSetLayout;
    }
    
    SpriteBatch::~SpriteBatch(){
    }
}