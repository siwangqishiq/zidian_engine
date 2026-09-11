#pragma once

#include <vector>

namespace zidian{
    class ICmd;

    class Batch{
    public:
        virtual void flush();
        virtual void addCmd(ICmd *cmd);
        virtual ~Batch();
    private:
        std::vector<ICmd *> cmdList;
    };
}