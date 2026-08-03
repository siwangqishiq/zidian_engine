#pragma once

#include "renderer/render.h"

namespace zidian{
    class FrameResource{
    public:
        FrameResource(Render &context);
        ~FrameResource();
    private:
        Render &ctx;
    };
}

