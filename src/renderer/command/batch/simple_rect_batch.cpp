#include "renderer/command/batch/simple_rect_batch.h"
#include "utils/log.h"

namespace zidian{
    SimpleRectBatch::SimpleRectBatch(Render &ctx_) : Batch(ctx_){
        pipeline = std::make_unique<SimpleRectPipeline>(ctx, *ctx.pipelineManager);
        pipeline->create();
        Log::green("pipeline", "create simple_rect pipeline success!");
    }

    void SimpleRectBatch::init(){
    }

    bool SimpleRectBatch::canBatch(const Cmd& cmd){
        if(cmd.type != CmdType::DrawSimpleRect){
            return false;
        }

        return true;
    }

    void SimpleRectBatch::putCmd(const Cmd& cmd){
        
    }

    void SimpleRectBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex){
        
    }

    void SimpleRectBatch::reset(){

    }

    SimpleRectBatch::~SimpleRectBatch(){
    }
}