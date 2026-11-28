#include "renderer/pipeline/pipe_simple_triangle.h"

#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/command/data/data_simple_triangle.h"
#include "renderer/command/data/common_uniform.h"

#include "utils/log.h"

namespace zidian{
    SimpleTrianglePipeline::SimpleTrianglePipeline(Render &context, PipelineManager &pipelineManager) 
        : BasePipeline::BasePipeline(context, pipelineManager){
    }

    SimpleTrianglePipeline::~SimpleTrianglePipeline(){
        dispose();
        Log::i("SimpleTrianglePipeline","SimpleTrianglePipeline::~SimpleTrianglePipeline");
    }

    VkShaderModule SimpleTrianglePipeline::getVertexShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_triangle.vert.spv");
    }
    
    VkShaderModule SimpleTrianglePipeline::getFragmentShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_triangle.frag.spv");
    }

    void SimpleTrianglePipeline::populateVertexInputState(){
        vertexInputBind = SimpleTriangleVertex::bindingDesc();
        vertexInputDescs = SimpleTriangleVertex::attributeDesc();

        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();

        Log::i("SimpleTrianglePipeline","SimpleTrianglePipeline::populateVertexInputState");
    }

    void SimpleTrianglePipeline::populateInputAssemblyState(){
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

        Log::i("SimpleTrianglePipeline","SimpleTrianglePipeline::populateInputAssemblyState");
    }

    bool SimpleTrianglePipeline::createPipelineLayout(){
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
            Log::e("SimpleTrianglePipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    std::vector<VkDescriptorSetLayout> SimpleTrianglePipeline::genDescriptorSetLayouts() {
        return {
            ctx.frameResource->getGlobalDescriptorSetLayout()
        };
    }
}