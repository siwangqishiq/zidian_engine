#include "renderer/batch/primitive_batch.h"

namespace zidian {
    PrimitiveBatch::PrimitiveBatch(Render &context) : ctx(context) {
    }

    void PrimitiveBatch::flush(){
        //vulkan draw
    }
}