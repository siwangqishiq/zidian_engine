#pragma once

#include "vulkan/vulkan.h"

namespace zidian {
    class PipelineManager;

    class PrimitivePipeline{
    public:
        PrimitivePipeline(PipelineManager &pipelineManager) : pipeMgr(pipelineManager){}

        void create();
        void dispose();
    private:
        PipelineManager& pipeMgr;
        VkPipeline pipeline = VK_NULL_HANDLE;
    };
}