
#include "renderer/render.h"
#include "utils/log.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/pipe_primitive.h"
#include "renderer/shader/shader_manager.h"
#include "renderer/pipeline/pipe_custom_shader.h"

namespace zidian {
    PipelineManager::PipelineManager(Render &context) : ctx(context){
    }

    PipelineManager::~PipelineManager(){
    }

    void PipelineManager::createPipelines() {
        createDescriptorPool();

        primitivePipe = std::make_unique<PrimitivePipeline>(ctx, *this);
        primitivePipe->create();
    }

    void PipelineManager::createDescriptorPool(){
        uint32_t pipelineTotalCount = 6 + customShaderPipelineCount; 
        uint32_t poolMultiplier = 4;

        VkDescriptorPoolSize poolSizes[] = {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                pipelineTotalCount * FrameResource::MAX_FRAME_IN_FLIGHT * poolMultiplier
            },
            {
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                FrameResource::MAX_FRAME_IN_FLIGHT * poolMultiplier
            }
        };

        VkDescriptorPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.poolSizeCount =  sizeof(poolSizes) / sizeof(VkDescriptorPoolSize);
        poolCreateInfo.pPoolSizes = poolSizes;
        poolCreateInfo.maxSets = pipelineTotalCount * FrameResource::MAX_FRAME_IN_FLIGHT * poolMultiplier;

        if(vkCreateDescriptorPool(ctx.device, &poolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS){
            Log::e("pipeline","create descriptor pool error!");
            return;
        }
        Log::i("pipeline","create descriptor pool success!");
    }

    CustomShaderPipelineId PipelineManager::createCustomShaderPipeline(std::string moduleName){
        std::shared_ptr<CustomShaderPipeline> pipeline = std::make_shared<CustomShaderPipeline>(ctx, *this, moduleName);
        pipeline->create();
        
        auto pid = pipeline->getCustomPipelineId();
        customShaderPipeMap[pid] = pipeline;
        incrementCustomPipelineCount();
        return pid;
    }

    void PipelineManager::updateDescriptorSets(){
        primitivePipe->updateDescriptorSet();
    }
    
    void PipelineManager::clearPipelines(){
        customShaderPipeMap.clear();

        primitivePipe->dispose();
        
        if(descriptorPool != VK_NULL_HANDLE){
            vkDestroyDescriptorPool(ctx.device, descriptorPool, nullptr);
        }
    }

    void PipelineManager::incrementCustomPipelineCount(){
        customShaderPipelineCount++;
    }

    std::shared_ptr<CustomShaderPipeline> PipelineManager::findCustomPipeline(CustomShaderPipelineId pid) const{
        return customShaderPipeMap.at(pid);
    }
}

