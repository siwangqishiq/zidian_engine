#include "renderer/command/batch/triangle_batch.h"
#include "renderer/pipeline/pipe_triangle.h"
#include "renderer/command/data/data_triangles.h"
#include "utils/log.h"

namespace zidian{
    TriangleBatch::TriangleBatch(Render &ctx_) : Batch(ctx_) {
        attachPipeline = std::make_unique<TrianglePipeline>(ctx, *ctx.pipelineManager);
        attachPipeline->create();
        Log::green("pipeline", "create TrianglePipeline success!");
    }

    void TriangleBatch::init(){
        createBuffers();
        Log::green("pipeline", "create Triangle buffer success!");
    }

    VkDeviceSize TriangleBatch::genBufferSize(){
        const uint32_t MAX_VERTEX_SIZE = 16 * 1024;
        return sizeof(TriangleVertex) * MAX_VERTEX_SIZE;
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

        ctx.drawCallCount++;
    }
    
    void TriangleBatch::reset(uint32_t frameIndex) {
        offsets[frameIndex] = 0;
    }
    
    TriangleBatch::~TriangleBatch(){
        Log::purple("batch", "TriangleBatch destroy buffer");
    }
}