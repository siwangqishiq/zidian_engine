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
        static constexpr uint32_t MAX_TRANSFORM_COUNT = 4 * 1024;

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
        // std::vector<uint32_t> primitiveVertexMaxCounts;
        // std::vector<VkBuffer> primitiveVertexBuffers;
        // std::vector<VkDeviceMemory> primitiveVertexMemorys;
        // std::vector<void *> primitiveMemoryMappeds;

        // push constant data
        std::vector<PushConstantData> pushConstDatas;

        //primitive uniform buffer
        // std::vector<VkDeviceMemory> primitiveUniformMemorys;
        // std::vector<VkBuffer> primitiveUniformBuffers;
        // std::vector<void *> primitiveUniformMemoryMappeds;

        std::vector<PrimitiveUniformData> primitiveUniformDatas;

        // Transform SSBO
        std::vector<uint32_t> transformMaxCounts;

        std::vector<VkBuffer> transformBuffers;
        std::vector<VkDeviceMemory> transformMemorys;
        std::vector<void *> transformMemoryMappeds;

        // CPU 侧 Transform 数据
        std::vector<std::vector<glm::mat4>> transformDatas;
    private:
        Render &ctx;

        void createCommandBuffers();
        void createSyncObjects();

        void createPushConstantDatas();

        // void createUniformDatas();
        // void createPrimitiveUniformBuffers();

        void createTransformBuffers();
        void uploadTransformData(const uint32_t frameIndex);

        void destroyTransformBuffers();
    public:
        // void createPrimitiveVertexBuffers();
        // bool createPrimitiveVertexBuffer(const uint32_t index);

        void createFramebuffers();
        void destroyFramebuffers();
    };
}

