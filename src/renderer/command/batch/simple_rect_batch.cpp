#include "renderer/command/batch/simple_rect_batch.h"

namespace zidian{
    SimpleRectBatch::SimpleRectBatch(Render &ctx_) : Batch(ctx_){
        pipeline = std::make_unique<SimpleRectPipeline>(ctx, *ctx.pipelineManager);
        pipeline->create();
    }

    void SimpleRectBatch::init(){
    }

    bool SimpleRectBatch::canBatch(const Cmd& cmd){
        return false;
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