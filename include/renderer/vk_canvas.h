#pragma once

#include "renderer/canvas.h"
#include "renderer/render.h"

namespace zidian {
    class VkCanvas : public ICanvas{
    public:
        VkCanvas(Render &context);

        virtual void drawTriangles(const glm::vec2 *vertices,const glm::vec4 *colors,const uint32_t vertexCount) override;

        virtual void flush() override;

        virtual ~VkCanvas();
    private:
        Render &ctx;
    };
}


