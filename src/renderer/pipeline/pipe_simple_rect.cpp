#include "renderer/pipeline/pipe_simple_rect.h"

#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/command/data/data_simple_rect.h"
#include "renderer/command/data/common_uniform.h"

#include "utils/log.h"

namespace zidian{
    SimpleRectPipeline::SimpleRectPipeline(Render &context, PipelineManager &pipelineManager) 
        : BasePipeline::BasePipeline(context, pipelineManager){
    }

    SimpleRectPipeline::~SimpleRectPipeline(){
        dispose();
        Log::i("SimpleRectPipeline","SimpleRectPipeline::~SimpleRectPipeline");
    }

    VkShaderModule SimpleRectPipeline::getVertexShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_rect.vert.spv");
    }
    
    VkShaderModule SimpleRectPipeline::getFragmentShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_rect.frag.spv");
    }

    void SimpleRectPipeline::populateVertexInputState(){
        vertexInputBind = SimpleRectVertex::bindingDesc();
        vertexInputDescs = SimpleRectVertex::attributeDesc();

        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();

        Log::i("SimpleRectPipeline","SimpleRectPipeline::populateVertexInputState");
    }

    void SimpleRectPipeline::populateInputAssemblyState(){
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

        Log::i("SimpleRectPipeline","SimpleRectPipeline::populateInputAssemblyState");
    }

    bool SimpleRectPipeline::createPipelineLayout(){
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        const std::vector<VkDescriptorSetLayout> descriptorSetLayouts = genDescriptorSetLayouts();
        if(descriptorSetLayouts.empty()){
            layoutCreateInfo.setLayoutCount = 0;
            layoutCreateInfo.pSetLayouts = nullptr;
        }else{
            layoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
            layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
        }
        
        VkPushConstantRange range{};
        range.offset = 0;
        range.size = sizeof(CommonUniform);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &range;
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("primitive_pipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    std::vector<VkDescriptorSetLayout> SimpleRectPipeline::genDescriptorSetLayouts() {
        return {
            ctx.frameResource->getGlobalDescriptorSetLayout()
        };
    }
}