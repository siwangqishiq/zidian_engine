#include "renderer/mem/device_memory.h"
#include "renderer/mem/memory_allocator.h"

namespace zidian {

    bool MemoryAllocator::init(VkPhysicalDevice physicalDevice, VkDevice device){
        this->physicalDevice = physicalDevice;
        this->device = device;
        return true;
    }

    void MemoryAllocator::destroy(){
        for(auto memory : memories){
            memory->free();
            delete memory;
        }//end for each
        memories.clear();
    }

    Allocation MemoryAllocator::allocate(const VkMemoryRequirements& requirements, MemoryType memoryType){
        Allocation allocation;
        VkMemoryPropertyFlags properties = 0;

        switch(memoryType){
        case MemoryType::HostVisible:
            properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case MemoryType::DeviceLocal:
            properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        }//end switch

        uint32_t memoryTypeIndex = findMemoryType(requirements.memoryTypeBits, properties);
        if(memoryTypeIndex == UINT32_MAX){
            return allocation;
        }

        DeviceMemory *memory = new DeviceMemory();
        if(!memory->allocate(device, requirements.size, memoryTypeIndex)){
            delete memory;
            return allocation;
        }

        memories.push_back(memory);

        allocation.memory = memory;
        allocation.offset = 0;
        allocation.size = requirements.size;
        return allocation;
    }

    uint32_t MemoryAllocator::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
        VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++){
            if((typeFilter & (1u << i)) 
                && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){
                return i;
            }
        }
        return UINT32_MAX;
    }
}