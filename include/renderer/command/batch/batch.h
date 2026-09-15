#pragma once

#include "vulkan/vulkan.h"

namespace zidian{
    class Cmd;

    class Batch{
    public:
        virtual bool canBatch(const Cmd& cmd) = 0;
        virtual void putCmd(const Cmd& cmd) = 0;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) = 0;
        virtual void reset() = 0;
        virtual ~Batch(){};
    };
}