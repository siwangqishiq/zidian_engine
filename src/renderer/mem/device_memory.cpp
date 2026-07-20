#include "renderer/mem/device_memory.h"
#include "utils/log.h"


namespace zidian {
    bool DeviceMemory::allocate(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex){
        this->device = device;
        this->size = size;

        VkMemoryAllocateInfo allocatInfo{};
        allocatInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocatInfo.allocationSize = size;
        allocatInfo.memoryTypeIndex = memoryTypeIndex;

        if(vkAllocateMemory(device, &allocatInfo, nullptr, &memory) != VK_SUCCESS){
            Log::e("memory", "allocator memory failed!");
            return false;
        }
        return true;
    }

    void DeviceMemory::free(){
        if(memory == VK_NULL_HANDLE){
            return;
        }

        vkFreeMemory(device, memory, nullptr);
        memory = VK_NULL_HANDLE;
        size = 0;
    }

    void* DeviceMemory::map(VkDeviceSize offset,VkDeviceSize size){
        if(memory == VK_NULL_HANDLE){
            return nullptr;
        }

        void *mappedData = nullptr;
        if(vkMapMemory(device, memory, 0, size, 0, &mappedData) != VK_SUCCESS){
            Log::e("memory", "mapped memory failed!");
            return nullptr;
        }
        return mappedData;
    }

    void DeviceMemory::unmap(){
        if(memory != VK_NULL_HANDLE){
            vkUnmapMemory(device, memory);
        }
    }
}


