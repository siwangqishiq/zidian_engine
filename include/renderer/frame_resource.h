#pragma once

#include <vector>
#include "vulkan/vulkan.h"
#include "renderer/pipeline/push_constant_data.h"
#include "renderer/pipeline/primitive_uniform_data.h"

namespace zidian{
    class Render;

    class FrameResource{
    public:
        static uint32_t MAX_FRAME_IN_FLIGHT;

        FrameResource(Render &context);

        void init();

        void destroy();

        ~FrameResource();

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFramebuffer> frameBuffers;
        //GPU同步对象
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishSemaphores;
        //CPU -> GPU 同步对象
        std::vector<VkFence> inFlightFences;

        //primitive vertices buffer 
        std::vector<uint32_t> primitiveVertexMaxCounts;
        std::vector<VkBuffer> primitiveVertexBuffers;
        std::vector<VkDeviceMemory> primitiveVertexMemorys;
        std::vector<void *> primitiveMemoryMappeds;

        // push constant data
        std::vector<PushConstantData> pushConstDatas;

        //primitive uniform buffer
        std::vector<VkDeviceMemory> primitiveUniformMemorys;
        std::vector<VkBuffer> primitiveUniformBuffers;
        std::vector<void *> primitiveUniformMemoryMappeds;

        std::vector<PrimitiveUniformData> primitiveUniformDatas;
    private:
        Render &ctx;

        void createCommandBuffers();
        void createSyncObjects();
        void createPushConstantDatas();
        void createUniformDatas();

        void createPrimitiveUniformBuffers();
    public:
        void createPrimitiveVertexBuffers();
        bool createPrimitiveVertexBuffer(const uint32_t index);

        void createFramebuffers();
        void destroyFramebuffers();
    };
}

