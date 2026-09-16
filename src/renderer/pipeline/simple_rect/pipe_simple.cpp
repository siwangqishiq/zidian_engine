#include "renderer/pipeline/simple_rect/pipe_simple_rect.h"

#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"

namespace zidian{
    SimpleRectPipeline::SimpleRectPipeline(Render &context, PipelineManager &pipelineManager) 
        : BasePipeline::BasePipeline(context, pipelineManager){
    }

    SimpleRectPipeline::~SimpleRectPipeline(){
    }

    VkShaderModule SimpleRectPipeline::getVertexShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_rect.vert.spv");
    }
    
    VkShaderModule SimpleRectPipeline::getFragmentShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_rect.frag.spv");
    }
}