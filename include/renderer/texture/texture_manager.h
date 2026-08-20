#pragma once

#include "vulkan/vulkan.h"

namespace zidian{
    class Render;

    class TextureManager{
    public:
        TextureManager(Render &context);

        ~TextureManager();
    private:
        Render &ctx;
    };
}