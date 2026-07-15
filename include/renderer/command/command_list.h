#pragma once

#include <vector>
#include "renderer/vertex/primitive_vertex.h"
#include "renderer/command/primitive_command.h"

namespace zidian {
    class CommandList{
    public:
        CommandList();
        ~CommandList();

        std::vector<PrimitiveVertex>& getPrimitiveVertices();
        std::vector<PrimitiveCommand>& getPrimitiveCommands();

    private:
        std::vector<PrimitiveVertex> primitiveVertices;
        std::vector<PrimitiveCommand> primitiveCommands;
    };
}



