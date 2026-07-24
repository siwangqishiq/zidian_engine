#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include "renderer/mem/allocation.h"

namespace zidian{

    class DeviceMemory;
    
    enum class MemoryType {
        HostVisible,
        DeviceLocal
    };

    class MemoryAllocator{
    public:
        MemoryAllocator() = default;

        ~MemoryAllocator() = default;

        bool init(VkPhysicalDevice physicalDevice, VkDevice deivce);

        void destroy();

        Allocation allocate(const VkMemoryRequirements& requirements, MemoryType memoryType);

        void free(const Allocation& allocation);
    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; 
        VkDevice device = VK_NULL_HANDLE;

        std::vector<DeviceMemory *> memories;

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };
}