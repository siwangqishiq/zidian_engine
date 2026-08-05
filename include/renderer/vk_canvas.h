#pragma once

#include "renderer/canvas.h"
#include "renderer/render.h"
#include "renderer/geometry.h"

namespace zidian {
    class VkCanvas : public ICanvas{
    public:
        VkCanvas(Render &context);

        virtual void drawTriangle(const glm::vec2 *vertices,const glm::vec4 color) override;

        virtual void drawTriangles(const glm::vec2 *vertices,const glm::vec4 *colors,const uint32_t vertexCount) override;

        virtual void drawRect(float left, float top, float width, float height, const glm::vec4 color) override;

        virtual void drawCircle(float cx, float cy, float radius, const glm::vec4 color) override;

        virtual void flush() override;

        virtual ~VkCanvas();

        virtual int getWidth() override;
        virtual int getHeight() override;
    private:
        Render &ctx;

        Geometry geometry;
    };
}


