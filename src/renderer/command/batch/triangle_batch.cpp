#include "renderer/command/batch/triangle_batch.h"

namespace zidian{
    TriangleBatch::TriangleBatch(Render &ctx_) : Batch(ctx_) {

    }

    bool TriangleBatch::canBatch(const Cmd& cmd) {
        if(cmd.type != CmdType::DrawTriangles){
            return false;
        }
        return true;
    }
    
    void TriangleBatch::putCmd(const Cmd& cmd, uint32_t frameIndex) {

    }
    
    void TriangleBatch::commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) {

    }
    
    void TriangleBatch::reset(uint32_t frameIndex) {

    }
    
    TriangleBatch::~TriangleBatch(){
        
    }
        
    void TriangleBatch::createBuffers(){

    }
}