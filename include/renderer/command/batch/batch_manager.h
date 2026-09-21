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

        void autoRegisterBatchs();

        void registerBatch(CmdType type, std::shared_ptr<Batch> batch);

        std::shared_ptr<Batch> findBatchByType(CmdType type);

        void freeAllBatch();
    private:
        Render &ctx;
        std::map<CmdType, std::shared_ptr<Batch>> batchMap; 
    };
}
