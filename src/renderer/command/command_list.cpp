#include "renderer/command/command_list.h"

namespace zidian {
    CommandList::CommandList(){
    }

    CommandList::~CommandList(){
    }


    std::vector<PrimitiveVertex>& CommandList::getPrimitiveVertices(){
        return primitiveVertices;
    }
    
    std::vector<PrimitiveCommand>& CommandList::getPrimitiveCommands(){
        return primitiveCommands;
    }
}


