#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/primitive_pipe.h"

namespace zidian{
    PrimitivePipeline::PrimitivePipeline(PipelineManager *pipelineManager){
        this->pipeMgr = pipelineManager;
    }

    void PrimitivePipeline::create() {
        
    }

    void PrimitivePipeline::dispose(){
        if(pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(pipeMgr->device, pipeline, nullptr);
        }
    }
}