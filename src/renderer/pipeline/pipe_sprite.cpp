#include "renderer/pipeline/pipe_sprite.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/render.h"
#include "utils/log.h"
#include "renderer/command/data/common_uniform.h"
#include "renderer/command/data/data_sprite.h"

namespace zidian{
    SpritePipeline::SpritePipeline(Render &context, PipelineManager &pipelineManager)
        : BasePipeline::BasePipeline(context, pipelineManager){
    }
    
    SpritePipeline::~SpritePipeline(){
        dispose();
        Log::i("SpritePipeline","SpritePipeline::~SpritePipeline");
    }
    
    VkShaderModule SpritePipeline::getVertexShaderModule() {
        return ctx.shaderManager->createShaderModule("shaders/sprite.vert.spv");
    }

    VkShaderModule SpritePipeline::getFragmentShaderModule() {
        return ctx.shaderManager->createShaderModule("shaders/sprite.frag.spv");
    }

    void SpritePipeline::populateVertexInputState() {
        vertexInputBind = SpriteVertex::bindingDesc();
        vertexInputDescs = SpriteVertex::attributeDesc();

        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();

        Log::i("SpritePipeline","SpritePipeline::populateVertexInputState");
    }

    void SpritePipeline::populateInputAssemblyState() {
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        
        Log::i("SpritePipeline","SpritePipeline::populateInputAssemblyState");
    }

    bool SpritePipeline::createPipelineLayout(){
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
        layoutCreateInfo.setLayoutCount = 1;
        layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("SpritePipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    void SpritePipeline::createDescriptorSetLayout(){
        std::array<VkDescriptorSetLayoutBinding, 1> uboLayoutBindings = SpriteUniform::layoutBindingDesc();

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = uboLayoutBindings.size();
        layoutCreateInfo.pBindings = uboLayoutBindings.data();

        // Log::green("SpritePipeline", "create sprite pipeline descriptorSet layout");
        
        if(vkCreateDescriptorSetLayout(ctx.device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS){
            Log::e("SpritePipeline", "create descriptor set layout failed!");
            return;
        }
    }
}//end namespace
