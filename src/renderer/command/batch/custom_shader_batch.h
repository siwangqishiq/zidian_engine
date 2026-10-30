#pragma once

#include "renderer/command/batch/batch.h"
#include "renderer/command/data/data_custom_shader.h"
#include <memory>

namespace zidian{
    class CustomShaderBatch : public Batch{
    public:
        CustomShaderBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd, uint32_t frameIndex) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset(uint32_t frameIndex) override;
        virtual ~CustomShaderBatch();
        virtual VkDeviceSize genBufferSize() override;

    private:
        std::vector<CustomShaderVertex> vertexData;
        CustomShaderPipelineId currentPipelineId = -1;
    };
}