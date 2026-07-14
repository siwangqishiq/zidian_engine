#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include <string>
#include <map>

namespace zidian {
    class Render;

    class ShaderManager{
    public:
        explicit ShaderManager(Render &context);

        VkShaderModule createShaderModule(const std::string& path);

        void destroyShaderModule(VkShaderModule shaderModule);
        void clearAllShaderModules();
    private:
        Render &ctx;
        std::map<std::string, VkShaderModule> shaderModuleMap;

        bool removeShaderModuleFromMap(VkShaderModule &module);
    };
}