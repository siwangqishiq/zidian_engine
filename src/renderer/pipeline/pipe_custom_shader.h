#pragma once

#include "renderer/pipeline/pipe_base.h"

namespace zidian {
    class Render;

    class CustomShaderPipeline : public BasePipeline {
    public:
        CustomShaderPipeline(Render &context, PipelineManager &pipelineManager, std::string shaderName);
        virtual ~CustomShaderPipeline();

        virtual VkShaderModule getVertexShaderModule() override;
        virtual VkShaderModule getFragmentShaderModule() override;

        virtual void populateVertexInputState() override;
        virtual void populateInputAssemblyState() override;
        virtual bool createPipelineLayout() override;
        virtual void createDescriptorSetLayout() override;
    private:
        std::string shaderModuleName;
    };
}