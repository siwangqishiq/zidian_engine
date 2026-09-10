#include "renderer/pipeline/pipe_primitive.h"


namespace zidian{
    PrimitivePipeline::PrimitivePipeline(Render &context, PipelineManager &pipelineManager) 
        : BasePipeline(context, pipelineManager){
    }

    PrimitivePipeline::~PrimitivePipeline(){
    }
}