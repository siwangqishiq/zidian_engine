#pragma once

#include "renderer/command/batch/batch.h"
#include "renderer/command/data/data_triangles.h"
#include "renderer/pipeline/pipe_triangle.h"
#include <memory>

namespace zidian{
    class TriangleBatch : public Batch{
    public:
        TriangleBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd, uint32_t frameIndex) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset(uint32_t frameIndex) override;
        virtual ~TriangleBatch();

        virtual VkDeviceSize genBufferSize() override;
    private:
        std::unique_ptr<TrianglePipeline> attachPipeline;
        std::vector<TriangleVertex> vertexData;
    };
}