#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/primitive_pipe.h"

namespace zidian{
    void PrimitivePipeline::create() {
        
    }

    void PrimitivePipeline::dispose(){
        if(pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(pipeMgr.device, pipeline, nullptr);
        }
    }
}