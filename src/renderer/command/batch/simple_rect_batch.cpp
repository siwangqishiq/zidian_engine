#include "renderer/command/batch/simple_rect_batch.h"

namespace zidian{
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