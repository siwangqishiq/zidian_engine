#include "renderer/render.h"
#include "renderer/vk_canvas.h"
#include "renderer/pipeline/primitive_vertex.h"
#include "renderer/command/cmd.h"
#include "renderer/command/render_queue.h"
#include "glm/gtc/matrix_transform.hpp"
#include "utils/log.h"

namespace zidian {
    VkCanvas::VkCanvas(Render &context) : ctx(context) {
    }

    VkCanvas::~VkCanvas(){
    }

    void VkCanvas::drawTriangles(glm::vec2 *vertices, glm::vec4 *colors,const uint32_t vertexCount){
        if(vertexCount == 0){
            return;
        }

        Cmd cmd;
        cmd.type = CmdType::DrawTriangles;
        DrawTrianglesData data;
        data.vertexCount = vertexCount;
        data.vertices.resize(vertexCount);
        data.colors.resize(vertexCount);
        
        for(int i = 0 ; i < vertexCount ; i++){
            data.vertices[i] = vertices[i];
            data.colors[i] = colors[i];
        }//end for i
        data.transformMatrixIndex = getMatrixIndex();
        // Log::purple("canvasdebug", "transformMatrixIndex = %u", data.transformMatrixIndex);
        
        cmd.data = data;
        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::drawTriangle(glm::vec2 *vertices,glm::vec4 color){
        const uint32_t vertexCount = 3;
        glm::vec4 colors[vertexCount] = {color,color,color};
        drawTriangles(vertices, colors, vertexCount);
    }

    int VkCanvas::getWidth() {
        return ctx.swapChainExtent.width;
    }

    int VkCanvas::getHeight() {
        return ctx.swapChainExtent.height;
    }
    
    void VkCanvas::drawRect(float left, float top, float width, float height, const glm::vec4 color) {
        auto vertices = geometry.genRectVertices(left, top, width , height);
        const uint32_t vertexCount = vertices.size();
        glm::vec4 colors[] = {color,color,color,color,color,color};
        drawTriangles(vertices.data(), colors, vertexCount);
    }

    void VkCanvas::drawCircle(float cx, float cy, float radius, const glm::vec4 color) {
        auto vertices = geometry.genCircleVertices(cx, cy, radius);
        const uint32_t vertexCount = vertices.size();
        std::vector<glm::vec4> colors(vertexCount, color);
        drawTriangles(vertices.data(), colors.data(), vertexCount);
    }

    void VkCanvas::drawLines(glm::vec2 *points, uint32_t pointsCount, float lineWidth, const glm::vec4 color) {
        if(points == nullptr || pointsCount <= 1){
            return;
        }

        auto vertices = geometry.genLinesVertices(points, pointsCount, lineWidth);
        const uint32_t vertexCount = vertices.size();
        std::vector<glm::vec4> colors(vertexCount, color);
        drawTriangles(vertices.data(), colors.data(), vertexCount);
    }

    void VkCanvas::drawLine(glm::vec2 start, glm::vec2 end,float lineWidth, const glm::vec4 color) {
        const uint32_t pointCount = 2;
        std::vector<glm::vec2> points(pointCount);
        points[0] = start;
        points[1] = end;
        return drawLines(points.data(), pointCount, lineWidth, color);
    }

    void VkCanvas::drawBezier2(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2, float lineWidth, const glm::vec4 color) {
        auto points = geometry.genBezier2Vertices(p0,p1,p2);
        drawLines(points.data(), points.size(), lineWidth, color);
    }
    
    void VkCanvas::drawBezier3(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2, glm::vec2 p3, float lineWidth, const glm::vec4 color) {
        auto points = geometry.genBezier3Vertices(p0,p1,p2,p3);
        drawLines(points.data(), points.size(), lineWidth, color);
    }

    void VkCanvas::drawSimpleRect(float left, float top, float width, float height, const glm::vec4 color){
        Cmd cmd;
        cmd.type = CmdType::DrawSimpleRect;
        DrawSimpleRectData data{.left = left, .top = top, .width = width , .height = height,.color = color};
        data.transformMatrixIndex = getMatrixIndex();
        cmd.data = data;
        
        ctx.renderQueue->addCmd(cmd);
    }
    
    void VkCanvas::drawSimpleTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4 color){
        Cmd cmd;
        cmd.type = CmdType::DrawSimpleTriangle;
        DrawSimpleTriangleData data{.p1 = p1, .p2 = p2, .p3 = p3,.color = color};
        cmd.data = data;

        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::drawSimpleCircle(glm::vec2 center, float radius, const glm::vec4 color){
        Cmd cmd;
        cmd.type = CmdType::DrawSimpleCircle;
        DrawSimpleCircleData data = {.center = center , .radius = radius , .color = color};
        cmd.data = data;

        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect){
        drawImage(image, srcRect , dstRect, glm::vec4(1.0f , 1.0f , 1.0f , 1.0f));
    }

    void VkCanvas::drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect, const glm::vec4 color){
       drawImage(image, srcRect , dstRect, color, glm::mat4(1.0f));
    }

    void VkCanvas::drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect, const glm::vec4 color, glm::vec2 rotateCenter, float rotateDegree) {
        glm::mat4 matrix(1.0f);
        if (rotateDegree != 0.0f){
            float radian = -glm::radians(rotateDegree);
            glm::mat4 T1 = glm::translate(glm::mat4(1.0f), -glm::vec3(rotateCenter,0.0f));
            glm::mat4 R  = glm::rotate(glm::mat4(1.0f), radian , {0.0f, 0.0f , 1.0f});
            glm::mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3(rotateCenter,0.0f));
            matrix = T2 * R * T1 * matrix;
        }
        drawImage(image, srcRect , dstRect, color, matrix);
    }

    void VkCanvas::drawImage(std::shared_ptr<Image> image, Rect srcRect, Rect dstRect, const glm::vec4 color, const glm::mat4 &matrix) {
        Cmd cmd;
        cmd.type = CmdType::DrawSprite;

        DrawSpriteData data;
        data.image = image;
        data.srcRect = srcRect;
        data.dstRect = dstRect;
        data.matrix = matrix;
        data.color = color;
        data.transformMatrixIndex = getMatrixIndex();

        cmd.data = data;
        
        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::drawCustomShader(CustomShaderPipelineId pid, float left, float top, float width, float height) {
        Cmd cmd;
        cmd.type = CmdType::DrawCustomShader;
        
        DrawCustomShaderData data;
        data.pipeId = pid;
        data.dstRect = Rect{left, top , width , height};
        cmd.data = data;

        ctx.renderQueue->addCmd(cmd);
    }
    
    uint32_t VkCanvas::addTransform(const glm::mat4& matrix) {
        return ctx.addTransform(matrix);
    }

    void VkCanvas::flush(){
    }
}


