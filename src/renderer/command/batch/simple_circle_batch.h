#pragma once

#include "renderer/command/batch/batch.h"
#include "renderer/command/data/data_simple_circle.h"
#include "renderer/pipeline/pipe_simple_circle.h"
#include <memory>

namespace zidian{
    class SimpleCircleBatch : public Batch{
    public:
        SimpleCircleBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd, uint32_t frameIndex) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset(uint32_t frameIndex) override;
        virtual VkDeviceSize genBufferSize() override;
        virtual ~SimpleCircleBatch();
    private:
        std::unique_ptr<SimpleCirclePipeline> attachPipeline;

        std::vector<SimpleCircleVertex> vertexData;
    };
}



