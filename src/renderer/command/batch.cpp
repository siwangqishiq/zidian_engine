#include "renderer/command/batch.h"

namespace zidian{
    void Batch::flush(){
        cmdList.clear();
    }
    
    void Batch::addCmd(ICmd *cmd){
        cmdList.emplace_back(cmd);
    }
    
    Batch::~Batch(){
    }
}


