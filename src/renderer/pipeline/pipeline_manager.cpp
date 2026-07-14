
#include "renderer/render.h"
#include "renderer/pipeline/pipeline_manager.h"
#include "renderer/pipeline/primitive_pipe.h"
#include "renderer/shader/shader_manager.h"


namespace zidian {
    PipelineManager::PipelineManager(Render &context) : ctx(context){
    }

    PipelineManager::~PipelineManager(){
    }

    void PipelineManager::createPipelines() {
        primitivePipe = std::make_unique<PrimitivePipeline>(ctx);
        primitivePipe->create();
    }
    
    void PipelineManager::clearPipelines(){
        primitivePipe->dispose();
    }
}

