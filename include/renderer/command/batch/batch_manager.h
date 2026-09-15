#pragma once

#include <map>
#include <memory>
#include "renderer/command/cmd.h"
#include "renderer/command/batch/batch.h"

namespace zidian{
    class Render;

    class BatchManager{
    public:
        BatchManager(Render &context);
        ~BatchManager();

        
    private:
        Render &ctx;
        std::map<CmdType, std::unique_ptr<Batch>> batchMap; 
    };
}
