#include "renderer/pipeline/simple/pipe_simple_circle.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/command/data/common_uniform.h"

#include "utils/log.h"

namespace zidian{
    SimpleCirclePipeline::SimpleCirclePipeline(Render &context, PipelineManager &pipelineManager) 
        : BasePipeline::BasePipeline(context, pipelineManager){
    }

    SimpleCirclePipeline::~SimpleCirclePipeline(){
        dispose();
        Log::i("SimpleCirclePipeline","SimpleCirclePipeline::~SimpleCirclePipeline");
    }

    VkShaderModule SimpleCirclePipeline::getVertexShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_circle.vert.spv");
    }
    
    VkShaderModule SimpleCirclePipeline::getFragmentShaderModule(){
        return ctx.shaderManager->createShaderModule("shaders/simple_circle.frag.spv");
    }

    void SimpleCirclePipeline::populateVertexInputState(){
        vertexInputBind = SimpleCircleVertex::bindingDesc();
        vertexInputDescs = SimpleCircleVertex::attributeDesc();

        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();

        Log::i("SimpleCirclePipeline","SimpleCirclePipeline::populateVertexInputState");
    }

    void SimpleCirclePipeline::populateInputAssemblyState(){
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

        Log::i("SimpleCirclePipeline","SimpleCirclePipeline::populateInputAssemblyState");
    }

    bool SimpleCirclePipeline::createPipelineLayout(){
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
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("primitive_pipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    void SimpleCirclePipeline::createDescriptorSetLayout(){
        std::array<VkDescriptorSetLayoutBinding, 1> uboLayoutBindings = CommonUniform::bindingDesc();
        
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = uboLayoutBindings.size();
        layoutCreateInfo.pBindings = uboLayoutBindings.data();

        if(vkCreateDescriptorSetLayout(ctx.device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS){
            Log::e("primitive_pipeline", "create descriptor set layout failed!");
            return;
        }

        std::vector<VkDescriptorSetLayout> layouts(FrameResource::MAX_FRAME_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pipelineMgr.descriptorPool;
        allocInfo.descriptorSetCount = layouts.size();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(layouts.size());
        VkResult result = vkAllocateDescriptorSets(ctx.device, &allocInfo, descriptorSets.data());
        if(result != VK_SUCCESS){
            Log::e("primitive_pipeline", "create descriptor set objects failed! %d", result);
            return;
        }
    }
}