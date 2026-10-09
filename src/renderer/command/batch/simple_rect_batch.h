#include "renderer/command/batch/batch.h"
#include "renderer/pipeline/simple/pipe_simple_rect.h"
#include "renderer/command/data/data_simple_rect.h"
#include <memory>

namespace zidian{
    class SimpleRectBatch : public Batch{
    public:
        SimpleRectBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd, uint32_t frameIndex) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset(uint32_t frameIndex) override;
        virtual VkDeviceSize genBufferSize() override;
        virtual ~SimpleRectBatch();

    private:
        std::unique_ptr<SimpleRectPipeline> attachPipeline;

        std::vector<SimpleRectVertex> vertexData;
    };
}



