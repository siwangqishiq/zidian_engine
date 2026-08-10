#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/primitive_pipe.h"
#include "renderer/render.h"
#include "renderer/shader/shader_manager.h"
#include "utils/log.h"
#include "renderer/pipeline/push_constant_data.h"
#include "renderer/pipeline/primitive_uniform_data.h"


namespace zidian{
    PrimitivePipeline::PrimitivePipeline(Render &context, PipelineManager &pipelineManager) 
        : ctx(context), pipelineMgr(pipelineManager){
    }

    void PrimitivePipeline::create() {
        VkShaderModule vertShaderModule = ctx.shaderManager->createShaderModule("shaders/primitive.vert.spv");
        VkShaderModule fragShaderModule = ctx.shaderManager->createShaderModule("shaders/primitive.frag.spv");

        if(vertShaderModule == VK_NULL_HANDLE || fragShaderModule == VK_NULL_HANDLE){
            Log::e("pipeline", "shader module create failed, abort pipeline creation.");
            if(vertShaderModule != VK_NULL_HANDLE) ctx.shaderManager->destroyShaderModule(vertShaderModule);
            if(fragShaderModule != VK_NULL_HANDLE) ctx.shaderManager->destroyShaderModule(fragShaderModule);
            return;
        }

        VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
        vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageCreateInfo.module = vertShaderModule;
        vertShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
        fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageCreateInfo.module = fragShaderModule;
        fragShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo , fragShaderStageCreateInfo};

        //顶点输入描述符
        populateVertexInputState();
        //图元装配形式设定
        populateInputAssemblyState(); 
        //视口设置
        populateViewportState();
        //光栅化设置
        populateRasterizationState();
        //多重采样设置
        populateMultisampleState();
        //混合模式设置blend
        populateColorBlendState();
        
        //创建layout
        bool createLayoutSuccess = createPipelineLayout();
        if(!createLayoutSuccess){
            ctx.shaderManager->destroyShaderModule(vertShaderModule);
            ctx.shaderManager->destroyShaderModule(fragShaderModule);
            return;
        }

        //深度模板状态
        populateDepthStencilState();

        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = shaderStages;
        pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo; 
        pipelineCreateInfo.pInputAssemblyState = &inputAssembleCreateInfo;
        pipelineCreateInfo.pViewportState = &viewportCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
        pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        pipelineCreateInfo.pDynamicState = nullptr;
        pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = ctx.renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

        if(vkCreateGraphicsPipelines(ctx.device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS){
            // throw std::runtime_error("create graph pipeline failed!");
            Log::e("pipeline", "create primitive pipeline failed!");
            ctx.shaderManager->destroyShaderModule(vertShaderModule);
            ctx.shaderManager->destroyShaderModule(fragShaderModule);
            return;
        }

        ctx.shaderManager->destroyShaderModule(vertShaderModule);
        ctx.shaderManager->destroyShaderModule(fragShaderModule);

        Log::green("pipeline", "create primitive pipeline success.");
    }

    void PrimitivePipeline::dispose(){
        if(pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(ctx.device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }

        if(pipelineLayout != VK_NULL_HANDLE){
            vkDestroyPipelineLayout(ctx.device, pipelineLayout, nullptr);
            pipelineLayout = VK_NULL_HANDLE;
        }

        if(descriptorSetLayout != VK_NULL_HANDLE){
            vkDestroyDescriptorSetLayout(ctx.device, descriptorSetLayout, nullptr);
            descriptorSetLayout = VK_NULL_HANDLE;
        }
    }

    PrimitivePipeline::~PrimitivePipeline(){
    }

    VkVertexInputBindingDescription PrimitiveVertex::bindingDesc(){
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDesc.stride = sizeof(PrimitiveVertex);
        return bindingDesc;
    }
    
    std::array<VkVertexInputAttributeDescription , 2> PrimitiveVertex::attributeDesc(){
        std::array<VkVertexInputAttributeDescription , 2> descs;

        //position
        descs[0].binding = 0;
        descs[0].location = 0;
        descs[0].offset = offsetof(PrimitiveVertex, position);
        descs[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        //color
        descs[1].binding = 0;
        descs[1].location = 1;
        descs[1].offset = offsetof(PrimitiveVertex, color);
        descs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        
        return descs;
    }

    //顶点描述
    void PrimitivePipeline::populateVertexInputState(){
        vertexInputBind = PrimitiveVertex::bindingDesc();
        vertexInputDescs = PrimitiveVertex::attributeDesc();
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBind;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexInputDescs.size();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputDescs.data();
    }


    void PrimitivePipeline::populateInputAssemblyState(){
        inputAssembleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembleCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssembleCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    void PrimitivePipeline::populateRasterizationState(){
        rasterCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterCreateInfo.depthClampEnable = VK_FALSE;
        rasterCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterCreateInfo.lineWidth = 1.0f;
        rasterCreateInfo.cullMode = VK_CULL_MODE_NONE;
        rasterCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterCreateInfo.depthBiasEnable = VK_FALSE;
    }

    void PrimitivePipeline::populateColorBlendState() {
        colorBlendAttach.colorWriteMask = VK_COLOR_COMPONENT_R_BIT 
            | VK_COLOR_COMPONENT_G_BIT 
            | VK_COLOR_COMPONENT_B_BIT 
            | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttach.blendEnable = VK_TRUE;
        colorBlendAttach.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttach.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttach.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttach.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttach.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttach.alphaBlendOp = VK_BLEND_OP_ADD;

        colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendCreateInfo.attachmentCount = 1;
        colorBlendCreateInfo.pAttachments = &colorBlendAttach;
        colorBlendCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendCreateInfo.blendConstants[0] = 0.0f;
        colorBlendCreateInfo.blendConstants[1] = 0.0f;
        colorBlendCreateInfo.blendConstants[2] = 0.0f;
        colorBlendCreateInfo.blendConstants[3] = 0.0f;
    }

    void PrimitivePipeline::populateMultisampleState(){
        multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }

    void PrimitivePipeline::populateDepthStencilState(){
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.depthTestEnable = VK_FALSE;
        depthStencilInfo.depthWriteEnable = VK_FALSE;
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilInfo.stencilTestEnable = VK_FALSE;
    }

    void PrimitivePipeline::populateViewportState() {
        viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = ctx.swapChainExtent.width;
        viewport.height = ctx.swapChainExtent.height;
        viewportCreateInfo.viewportCount = 1;
        viewportCreateInfo.pViewports = &viewport;

        scissor.offset = {0, 0};
        scissor.extent = ctx.swapChainExtent;
        viewportCreateInfo.scissorCount = 1;
        viewportCreateInfo.pScissors = &scissor;
    }

    bool PrimitivePipeline::createPipelineLayout(){
        createDescriptorSetLayout();

        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutCreateInfo.setLayoutCount = 1;
        layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        
        //set push constant
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);
        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &pushConstantRange;
        
        if(vkCreatePipelineLayout(ctx.device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            Log::e("primitive_pipeline", "create pipeline layout failed!");
            return false;
        }
        return true;
    }

    void PrimitivePipeline::createDescriptorSetLayout(){
        auto uboLayoutBindings = PrimitiveUniformData::bindingDescs();
        
        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = uboLayoutBindings.size();
        layoutCreateInfo.pBindings = uboLayoutBindings.data();

        if(vkCreateDescriptorSetLayout(ctx.device, &layoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS){
            Log::e("primitive_pipeline", "create descriptor set layout failed!");
            return;
        }

        //分配 DescriptorSet
        std::vector<VkDescriptorSetLayout> layouts(FrameResource::MAX_FRAME_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pipelineMgr.descriptorPool;
        allocInfo.descriptorSetCount = layouts.size();
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(layouts.size());
        if(vkAllocateDescriptorSets(ctx.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS){
            Log::e("primitive_pipeline", "create descriptor set objects failed!");
            return;
        }

       
    }

    //更新DescriptorSet
    void PrimitivePipeline::updateDescriptorSet(){
        if(ctx.frameResource->primitiveUniformBuffers.empty()){
            Log::e("primitive_pipeline", "updateDescriptorSet but uniform buffer is empty");
            return;
        }

        for(uint32_t i = 0 ; i < descriptorSets.size() ; i++){
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = ctx.frameResource->primitiveUniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(PrimitiveUniformData);

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = descriptorSets[i];
            write.dstBinding = 0;
            write.dstArrayElement = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(ctx.device, 1, &write, 0, nullptr);
        }//end for i
    }
}