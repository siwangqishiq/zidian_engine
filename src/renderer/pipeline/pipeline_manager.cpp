
#include "renderer/render.h"
#include "utils/log.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/primitive_pipe.h"
#include "renderer/shader/shader_manager.h"

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
        VkDescriptorPoolSize poolSizes[] = {
            {
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                FrameResource::MAX_FRAME_IN_FLIGHT
            }
        };

        VkDescriptorPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.poolSizeCount =  sizeof(poolSizes) / sizeof(VkDescriptorPoolSize);
        poolCreateInfo.pPoolSizes = poolSizes;
        poolCreateInfo.maxSets = FrameResource::MAX_FRAME_IN_FLIGHT;

        if(vkCreateDescriptorPool(ctx.device, &poolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS){
            Log::e("pipeline","create descriptor pool error!");
            return;
        }
        Log::i("pipeline","create descriptor pool success!");
    }


    void PipelineManager::updateDescriptorSets(){
        primitivePipe->updateDescriptorSet();
    }
    
    void PipelineManager::clearPipelines(){
        primitivePipe->dispose();

        if(descriptorPool != VK_NULL_HANDLE){
            vkDestroyDescriptorPool(ctx.device, descriptorPool, nullptr);
        }
    }
}

