#include "renderer/command/batch/batch.h"
#include "renderer/pipeline/simple_rect/pipe_simple_rect.h"
#include <memory>

namespace zidian{
    class SimpleRectBatch : public Batch{
    public:
        SimpleRectBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset() override;
        virtual ~SimpleRectBatch();

    private:
        std::unique_ptr<SimpleRectPipeline> pipeline;
    };
}



