#pragma once

#include "vulkan/vulkan.h"

namespace zidian{
    class PipelineManager{
    public:
        PipelineManager(VkDevice d) : device(d){}

        void createPipelines();
        void clearPipelines();
        
        VkDevice device;
    private:
    };
}

