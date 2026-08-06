#pragma once

#include <vector>
#include "vulkan/vulkan.h"
#include "renderer/pipeline/push_constant_data.h"

namespace zidian{
    class Render;

    class FrameResource{
    public:
        uint32_t MAX_FRAME_IN_FLIGHT = 2;

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
        std::vector<uint32_t> primitiveVertexMaxCounts;
        std::vector<VkBuffer> primitiveVertexBuffers;
        std::vector<VkDeviceMemory> primitiveVertexMemorys;
        std::vector<void *> primitiveMemoryMappeds;

        std::vector<PushConstantData> pushConstDatas;
    private:
        Render &ctx;

        void createCommandBuffers();
        void createSyncObjects();
        void createPushConstantDatas();

    public:
        void createPrimitiveVertexBuffers();
        bool createPrimitiveVertexBuffer(const uint32_t index);

        void createFramebuffers();
        void destroyFramebuffers();
    };
}

