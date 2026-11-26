#include "renderer/pipeline/pipe_triangle.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/render.h"
#include "renderer/command/data/data_triangles.h"
#include "utils/log.h"
#include "renderer/command/data/common_uniform.h"

namespace zidian{
    TrianglePipeline::TrianglePipeline(Render &context, PipelineManager &pipelineManager)
        : BasePipeline::BasePipeline(context, pipelineManager){
    }
    
    TrianglePipeline::~TrianglePipeline(){
        dispose();
        Log::i("TrianglePipeline","TrianglePipeline::~TrianglePipeline");
    }
    
    VkShaderModule TrianglePipeline::getVertexShaderModule() {
        return ctx.shaderManager->createShaderModule("shaders/triangle.vert.spv");
    }

    VkShaderModule TrianglePipeline::getFragmentShaderModule() {
        return ctx.shaderManager->createShaderModule("shaders/triangle.frag.spv");
    }

    void TrianglePipeline::populateVertexInputState() {
        vertexInputBind = TriangleVertex::bindingDesc();
        vertexInputDescs = TriangleVertex::attributeDesc();

        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();

        Log::i("TrianglePipeline","TrianglePipeline::populateVertexInputState");
    }

    void TrianglePipeline::populateInputAssemblyState() {
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        
        Log::i("TrianglePipeline","TrianglePipeline::populateInputAssemblyState");
    }

    bool TrianglePipeline::createPipelineLayout(){
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        std::vector<VkDescriptorSetLayout> setLayouts = genDescriptorSetLayouts();
        if(setLayouts.empty()){
            layoutCreateInfo.setLayoutCount = 0;
            layoutCreateInfo.pSetLayouts = nullptr;
        }else{
            layoutCreateInfo.setLayoutCount = setLayouts.size();
            layoutCreateInfo.pSetLayouts = setLayouts.data();
        }
        
        VkPushConstantRange range{};
        range.offset = 0;
        range.size = sizeof(CommonUniform);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &range;
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("TrianglePipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    std::vector<VkDescriptorSetLayout> TrianglePipeline::genDescriptorSetLayouts() {
        return {
            ctx.frameResource->getGlobalDescriptorSetLayout()
        };
    }
}//end namespace
