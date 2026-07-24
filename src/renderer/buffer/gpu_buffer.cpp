#include "renderer/buffer/gpu_buffer.h"

namespace zidian {
    GpuBuffer::~GpuBuffer(){
        destroy();
    }

    bool GpuBuffer::create(MemoryAllocator &alloc, VkDeviceSize size, VkBufferUsageFlags usage, MemoryType memoryType){
        this->allocator = &alloc;

        return false;
    }

    void GpuBuffer::destroy(){
        if(buffer != VK_NULL_HANDLE){
            vkDestroyBuffer(device, buffer, nullptr);
            buffer = VK_NULL_HANDLE;
        }

        if(allocator != nullptr 
                && allocation.memory != nullptr 
                && (allocation.memory) != VK_NULL_HANDLE){
        }

        bufferSize = 0;
    }

    bool GpuBuffer::upload(const void* data, VkDeviceSize size, VkDeviceSize offset){
        return false;
    }

    VkBuffer GpuBuffer::getBuffer() const{
        return buffer;
    }

    VkDeviceSize GpuBuffer::getSize() const {
        return bufferSize;
    }
}