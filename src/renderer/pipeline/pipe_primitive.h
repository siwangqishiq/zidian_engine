#pragma once

#include "renderer/pipeline/pipe_base.h"

namespace zidian {
    class Render;

    class PrimitivePipeline : public BasePipeline {
    public:
        PrimitivePipeline(Render &context, PipelineManager &pipelineManager);
        virtual ~PrimitivePipeline();
    };
}