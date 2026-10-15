#include "renderer/command/batch/batch.h"
#include "renderer/pipeline/pipe_simple_triangle.h"
#include "renderer/command/data/data_simple_rect.h"
#include "renderer/geometry.h"
#include <memory>

namespace zidian{
    class SimpleTriangleBatch : public Batch{
    public:
        SimpleTriangleBatch(Render &ctx_);

        virtual void init() override;
        virtual bool canBatch(const Cmd& cmd) override;
        virtual void putCmd(const Cmd& cmd, uint32_t frameIndex) override;
        virtual void commit(VkCommandBuffer &cmdBuffer,uint32_t frameIndex) override;
        virtual void reset(uint32_t frameIndex) override;
        virtual ~SimpleTriangleBatch();

        virtual VkDeviceSize genBufferSize() override;
    private:
        std::unique_ptr<SimpleTrianglePipeline> attachPipeline;

        std::vector<SimpleTriangleVertex> vertexData;

        Geometry geometry;
    };
}



