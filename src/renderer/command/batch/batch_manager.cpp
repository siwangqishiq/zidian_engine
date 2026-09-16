#include "renderer/command/batch/batch_manager.h"

namespace zidian{
    BatchManager::BatchManager(Render &context) : ctx(context){
    }

    BatchManager::~BatchManager(){
    }

    //将type与batch注册
    void BatchManager::registerBatch(CmdType type, std::shared_ptr<Batch> batch){
        batchMap[type] = batch;
    }

    std::shared_ptr<Batch> BatchManager::findBatchByType(CmdType type){
        auto it = batchMap.find(type);
        if (it != batchMap.end()) {
            return it->second;
        }
        return nullptr;
    }
}