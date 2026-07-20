#pragma once

#include "vulkan/vulkan.h"

namespace zidian{
    class DeviceMemory{
    public:
        DeviceMemory() = default;
        
        ~DeviceMemory() = default;

        bool allocate(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex);

        void free();

        void* map(VkDeviceSize offset = 0,VkDeviceSize size = VK_WHOLE_SIZE);

        void unmap();

        bool isValid() const{
            return memory != VK_NULL_HANDLE;
        }

        VkDeviceMemory getHandle() const {
            return memory;
        }

        VkDeviceSize getSize() const{
            return size;
        }
        
    private:
        VkDevice device = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDeviceSize size = 0;
    };
}