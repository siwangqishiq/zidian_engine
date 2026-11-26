#include "renderer/pipeline/pipe_sprite.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/render.h"
#include "utils/log.h"
#include "renderer/command/data/common_uniform.h"
#include "renderer/command/data/data_sprite.h"
#include "config.h"

namespace zidian{
    SpritePipeline::SpritePipeline(Render &context, PipelineManager &pipelineManager)
        : BasePipeline::BasePipeline(context, pipelineManager){
    }
    
    SpritePipeline::~SpritePipeline(){
        if(uniformDescSetLayout_ != VK_NULL_HANDLE){
            vkDestroyDescriptorSetLayout(ctx.device, uniformDescSetLayout_, nullptr);
            uniformDescSetLayout_ = VK_NULL_HANDLE;
        }
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
        VkPushConstantRange range{};
        range.offset = 0;
        range.size = sizeof(CommonUniform);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &range;

        descriptorSetLayouts_ = genDescriptorSetLayouts();
        if(descriptorSetLayouts_.empty()){
            layoutCreateInfo.setLayoutCount = 0;
            layoutCreateInfo.pSetLayouts = nullptr;
        }else{
            layoutCreateInfo.setLayoutCount = descriptorSetLayouts_.size();
            layoutCreateInfo.pSetLayouts = descriptorSetLayouts_.data();
        }
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("SpritePipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    std::vector<VkDescriptorSetLayout> SpritePipeline::genDescriptorSetLayouts(){
        // Log::purple("debug", "SpritePipeline::getDescriptorSetLayouts~~~");
        
        std::vector<VkDescriptorSetLayout> descSetLayouts;
        descSetLayouts.push_back(ctx.frameResource->getGlobalDescriptorSetLayout());

        auto uboLayoutBindings = SpriteUniform::layoutBindingDesc();
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = uboLayoutBindings.size();
        layoutCreateInfo.pBindings = uboLayoutBindings.data();

        if(vkCreateDescriptorSetLayout(ctx.device, &layoutCreateInfo, nullptr, &uniformDescSetLayout_) != VK_SUCCESS){
            Log::e("SpritePipeline", "create descriptor set layout failed!");
        }else{
            descSetLayouts.push_back(uniformDescSetLayout_);
        }

        if(IS_DEBUG){
            PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
                vkGetDeviceProcAddr(
                    ctx.device,
                    "vkSetDebugUtilsObjectNameEXT"
                )
            );

            VkDebugUtilsObjectNameInfoEXT nameInfo{};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
            nameInfo.objectHandle = reinterpret_cast<uint64_t>(uniformDescSetLayout_);
            nameInfo.pObjectName = "uniformDescSetLayout_SetLayout_zidian";
            vkSetDebugUtilsObjectNameEXT(ctx.device, &nameInfo);
        }

        return descSetLayouts;
    }

    std::vector<VkDescriptorSetLayout>& SpritePipeline::getDescriptorSetLayout(){
        return descriptorSetLayouts_;
    }

}//end namespace
