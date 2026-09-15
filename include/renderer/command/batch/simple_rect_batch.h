#include "renderer/command/batch/batch.h"

namespace zidian{
    class SimpleRectBatch : public Batch{
    public:
        virtual bool canBatch(const Cmd& cmd);
        virtual void putCmd(const Cmd& cmd);
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex);
        virtual void reset();
        virtual ~SimpleRectBatch();
    };
}



