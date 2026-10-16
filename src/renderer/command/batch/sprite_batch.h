#pragma once

#include "renderer/command/batch/batch.h"
#include "renderer/command/data/data_sprite.h"
#include "renderer/pipeline/pipe_sprite.h"
#include <memory>

namespace zidian{
    class SpriteBatch : public Batch{
    public:
        SpriteBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd, uint32_t frameIndex) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset(uint32_t frameIndex) override;
        virtual VkDescriptorSetLayout getDescriptorSetLayout() override;
        virtual ~SpriteBatch();

        virtual VkDeviceSize genBufferSize() override;
    private:
        std::unique_ptr<SpritePipeline> attachPipeline;
        std::vector<SpriteVertex> vertexData;
    };
}