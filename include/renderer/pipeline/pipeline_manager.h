#pragma once

#include <string>
#include "vulkan/vulkan.h"
#include <memory>

namespace zidian{
    class Render;
    class PrimitivePipeline;

    class PipelineManager{
    public:
        PipelineManager(Render &context);

        void createPipelines();

        void clearPipelines();

        ~PipelineManager();

        void updateDescriptorSets();

        void createDescriptorPool();

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        std::unique_ptr<PrimitivePipeline> primitivePipe;
    private:
        Render &ctx;
    };
}

