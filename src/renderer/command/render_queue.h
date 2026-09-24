#pragma once

#include "renderer/command/cmd.h"
#include <vector>

namespace zidian{
    class Render;

    class RenderQueue{
    public:
        RenderQueue(Render &context);
        ~RenderQueue();

        void addCmd(Cmd &cmd);
        void reset();

        std::vector<Cmd>& getCmdList();
    private:
        Render &ctx;

        std::vector<Cmd> cmdList;
    };
}