#pragma once

#include "vulkan/vulkan.h"
#include "renderer/render.h"
#include "renderer/pipeline/pipeline_manager.h"

namespace zidian{
    class Cmd;

    class Batch{
    public:
        Batch(Render &ctx_);
        
        virtual void init() = 0;
        virtual bool canBatch(const Cmd& cmd) = 0;
        virtual void putCmd(const Cmd& cmd) = 0;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) = 0;
        virtual void reset() = 0;
        virtual ~Batch(){};

        Render &ctx;
    };
}