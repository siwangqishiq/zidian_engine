#include "renderer/command/batch/batch_manager.h"
#include "utils/log.h"

#include "renderer/command/batch/batch.h"
#include "renderer/command/batch/simple_rect_batch.h"


namespace zidian{
    BatchManager::BatchManager(Render &context) : ctx(context){
    }

    BatchManager::~BatchManager(){
    }

    void BatchManager::autoRegisterBatchs(){
        std::shared_ptr<Batch> simpleRectBatch = std::make_shared<SimpleRectBatch>(ctx);
        registerBatch(CmdType::DrawSimpleRect, simpleRectBatch);
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

    void BatchManager::freeAllBatch(){
        batchMap.clear();
        Log::i("batch_manager", "freeAllBatch");
    }
}