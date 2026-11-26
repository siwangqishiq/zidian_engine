#pragma once

#include "glm/glm.hpp"
#include "renderer/geometry.h"
#include <memory>
#include "renderer/pipeline/pipe_common.h"

namespace zidian{
    class Image;

    //canvas状态
    struct CanvasState{
        glm::mat4 matrix{1.0f};
        uint32_t matrixIndex = UINT32_MAX;
        // float alpha = 1.0f;
    };
    
    class ICanvas{
    public:
        virtual void flush() = 0;

        //
        virtual void drawSimpleRect(float left, float top, float width, float height, const glm::vec4 color) = 0;
        virtual void drawSimpleTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4 color) = 0;
        virtual void drawSimpleCircle(glm::vec2 center, float radius, const glm::vec4 color) = 0;

        //基础图元绘制
        virtual void drawTriangles(glm::vec2 *vertices,glm::vec4 *colors,const uint32_t vertexCount) = 0;
        virtual void drawTriangle(glm::vec2 *vertices,glm::vec4 color) = 0;
        virtual void drawRect(float left, float top, float width, float height, const glm::vec4 color) = 0;
        virtual void drawCircle(float cx, float cy, float radius, const glm::vec4 color) = 0;
        virtual void drawLines(glm::vec2 *points, uint32_t pointCount, float lineWidth, const glm::vec4 color) = 0;
        virtual void drawLine(glm::vec2 start, glm::vec2 end,float lineWidth, const glm::vec4 color) = 0;
        //绘制二次贝塞尔曲线
        virtual void drawBezier2(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2, float lineWidth, const glm::vec4 color) = 0;
        //绘制三次贝塞尔曲线
        virtual void drawBezier3(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2, glm::vec2 p3, float lineWidth, const glm::vec4 color) = 0;

        // ======================== image =================================

        //
        virtual void drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect) = 0;
        virtual void drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect, const glm::vec4 color) = 0;
        virtual void drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect, const glm::vec4 color, glm::vec2 rotateCenter, float rotateDegree) = 0;
        virtual void drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect, const glm::vec4 color, const glm::mat4 &matrix) = 0;

        // custom shader 
        virtual void drawCustomShader(CustomShaderPipelineId pid, float left, float top, float width, float height) = 0;
        
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;

        // =========== canvas state ================
        void resetState();
        int save();
        void restore();
        void restoreToCount(int count);
        //平移变换
        void translate(float dx, float dy);

        //缩放变换
        void scale(float sx, float sy);
        void scale(float sx,float sy,float px,float py);

        // 旋转变换
        void rotate(float degrees);
        void rotate(float degrees,float px,float py);

        void concat(const glm::mat4& matrix);
        void setMatrix(const glm::mat4& matrix);

        virtual uint32_t addTransform(const glm::mat4& matrix) = 0;

        uint32_t getMatrixIndex();

        virtual ~ICanvas(){}
    private:
        std::vector<CanvasState> stateStack_;
        CanvasState state_;
    };//end canvas
}