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

        // push constant data
        std::vector<PushConstantData> pushConstDatas;

        std::vector<PrimitiveUniformData> primitiveUniformDatas;

        // Transform SSBO
        std::vector<uint32_t> transformMaxCounts;

        std::vector<VkBuffer> transformBuffers;
        std::vector<VkDeviceMemory> transformMemorys;
        std::vector<void *> transformMemoryMappeds;

        // CPU 侧 Transform 数据
        std::vector<std::vector<glm::mat4>> transformDatas;

        void uploadTransformData(const uint32_t frameIndex);

        std::vector<VkDescriptorSet> globalDescriptorSets;
    private:
        Render &ctx;

        VkDescriptorSetLayout globalDescSetLayout_ = VK_NULL_HANDLE;
        VkDescriptorPool globalDescSetPool_ = VK_NULL_HANDLE;

        void createCommandBuffers();
        void createSyncObjects();

        void createPushConstantDatas();

        void createTransformBuffers();

        void destroyTransformBuffers();

        void createGlobalDescriptorSetLayout();
        void createGlobalDescriptorSetPool();
        void allocAndUpdateGlobalDescriptorSet();
    public:
        void createFramebuffers();
        void destroyFramebuffers();
        void destroyGlobalDescriptorSetResources();

        VkDescriptorSetLayout& getGlobalDescriptorSetLayout();
    };
}

