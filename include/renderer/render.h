#pragma once

#include "vulkan/vulkan.h"

namespace zidian{
    class Render{
    public:
        void initVulkan();

        void onDispose();
    private:
        VkInstance instance;

        void createInstance();
    };
}