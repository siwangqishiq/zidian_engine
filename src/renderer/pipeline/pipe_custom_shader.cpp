#include "renderer/pipeline/pipe_custom_shader.h"
#include "utils/log.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/command/data/data_custom_shader.h"
#include "renderer/command/data/common_uniform.h"

namespace zidian{
    CustomShaderPipeline::CustomShaderPipeline(Render &context, PipelineManager &pipelineManager, std::string shaderName)
        : BasePipeline(context, pipelineManager), shaderModuleName(shaderName){
    }

    VkShaderModule CustomShaderPipeline::getVertexShaderModule() {
        return ctx.shaderManager->createShaderModule("shaders/" + shaderModuleName + ".vert.spv");
    }

    VkShaderModule CustomShaderPipeline::getFragmentShaderModule() {
        return ctx.shaderManager->createShaderModule("shaders/" + shaderModuleName + ".frag.spv");
    }
    
    CustomShaderPipeline::~CustomShaderPipeline(){
        dispose();
        Log::i("CustomShaderPipeline","CustomShaderPipeline::~CustomShaderPipeline");
    }

    void CustomShaderPipeline::populateVertexInputState(){
        vertexInputBind = CustomShaderVertex::bindingDesc();
        vertexInputDescs = CustomShaderVertex::attributeDesc();

        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();
        
        Log::i("CustomShaderPipeline","CustomShaderPipeline::populateVertexInputState");
    }

    void CustomShaderPipeline::populateInputAssemblyState(){
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        
        Log::i("CustomShaderPipeline","CustomShaderPipeline::populateInputAssemblyState");
    }

    bool CustomShaderPipeline::createPipelineLayout(){
        createDescriptorSetLayout();
        
        VkPushConstantRange range{};
        range.offset = 0;
        range.size = sizeof(CommonUniform);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pSetLayouts = nullptr;
        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &range;
        // layoutCreateInfo.setLayoutCount = 1;
        // layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("SpritePipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    void CustomShaderPipeline::createDescriptorSetLayout(){
    }
}