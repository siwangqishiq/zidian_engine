#include "renderer/command/render_queue.h"
#include "utils/log.h"

namespace zidian{
    RenderQueue::RenderQueue(Render &context) :  ctx(context){
        Log::i("render_queue", "render queue created!");
    }

    void RenderQueue::addCmd(Cmd &cmd){
        cmdList.emplace_back(cmd);
    }
    
    void RenderQueue::reset(){
        cmdList.clear();
    }

    std::vector<Cmd>& RenderQueue::getCmdList(){
        return cmdList;
    }

    RenderQueue::~RenderQueue(){
        Log::i("render_queue", "render queue destroyed!");
    }
}