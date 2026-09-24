#pragma once

#include "renderer/mem/memory_allocator.h"
#include "renderer/mem/allocation.h"

namespace zidian {
    class MemoryAllocator;

    class GpuBuffer{
    public:
        GpuBuffer() = default;
        virtual ~GpuBuffer();

        bool create(MemoryAllocator &allocator, VkDeviceSize size, VkBufferUsageFlags usage, MemoryType memoryType);
        
        void destroy();

        bool upload(const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

        VkBuffer getBuffer() const;

        VkDeviceSize getSize() const;
    protected:
        VkDevice device = VK_NULL_HANDLE;
        VkBuffer buffer = VK_NULL_HANDLE;
        Allocation allocation;
        VkDeviceSize bufferSize = 0;
        MemoryAllocator* allocator = nullptr;
    };
}