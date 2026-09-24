#pragma once

#include "vulkan/vulkan.h"

namespace zidian {
    class DeviceMemory;

    struct Allocation{
        DeviceMemory *memory = nullptr;
        VkDeviceSize size = 0;
        VkDeviceSize offset = 0;
        
        void* mapped = nullptr;
        uint32_t memoryTypeIndex = 0;   

        bool isValid() const{
            return memory != nullptr;
        }
    };
}