#include "renderer/command/command_list.h"

namespace zidian {
    CommandList::CommandList(){
    }

    CommandList::~CommandList(){
    }

    void CommandList::reset(){
        primitiveVertices.clear();
        primitiveCommands.clear();
    }

    std::vector<PrimitiveVertex>& CommandList::getPrimitiveVertices(){
        return primitiveVertices;
    }
    
    std::vector<PrimitiveCommand>& CommandList::getPrimitiveCommands(){
        return primitiveCommands;
    }
}


