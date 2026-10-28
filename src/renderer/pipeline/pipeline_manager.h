#pragma once

#include <string>
#include "vulkan/vulkan.h"
#include <memory>
#include <map>
#include "renderer/pipeline/pipe_common.h"

namespace zidian{
    class Render;
    class PrimitivePipeline;
    class CustomShaderPipeline;

    class PipelineManager{
    public:
        PipelineManager(Render &context);

        void createPipelines();

        void clearPipelines();

        ~PipelineManager();

        void createDescriptorPool();

        void updateDescriptorSets();

        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

        std::unique_ptr<PrimitivePipeline> primitivePipe;

        CustomShaderPipelineId createCustomShaderPipeline(std::string moduleName);

        void incrementCustomPipelineCount();
    private:
        Render &ctx;
        int customShaderPipelineCount = 0;
        std::map<CustomShaderPipelineId, std::shared_ptr<CustomShaderPipeline>> customShaderPipeMap;
    };
}

