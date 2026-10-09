#pragma once

#include "vulkan/vulkan.h"
#include "renderer/render.h"
#include "renderer/pipeline/pipeline_manager.h"
#include <vector>
#include "renderer/command/data/common_uniform.h"

namespace zidian{
    class Cmd;

    class Batch{
    public:
        Batch(Render &ctx_);
        
        virtual void init();
        virtual bool canBatch(const Cmd& cmd) = 0;
        virtual void putCmd(const Cmd& cmd ,uint32_t frameIndex) = 0;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) = 0;
        virtual void reset(uint32_t frameIndex) = 0;

        virtual VkDeviceSize genBufferSize() = 0;

        virtual void createBuffers();

        virtual ~Batch();
        virtual void dispose();

        Render &ctx;
    protected:
        std::vector<VkBuffer> vertexBuffers;
        std::vector<VkDeviceMemory> vertexMemorys;
        std::vector<void *> vertexMemoryMappeds;
        std::vector<uint32_t> offsets;
        std::vector<CommonUniform> pushConstantDatas;
    };
}