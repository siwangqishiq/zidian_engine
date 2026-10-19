#include "renderer/render.h"
#include "renderer/vk_canvas.h"
#include "renderer/pipeline/primitive_vertex.h"
#include "renderer/command/cmd.h"
#include "renderer/command/render_queue.h"

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
        glm::vec4 colors[vertexCount] = {color,color,color,color,color,color};
        drawTriangles(vertices.data(), colors, vertexCount);
    }

    void VkCanvas::drawCircle(float cx, float cy, float radius, const glm::vec4 color) {
        auto vertices = geometry.genCircleVertices(cx, cy, radius);
        const uint32_t vertexCount = vertices.size();
        std::vector<glm::vec4> colors(vertexCount, color);
        drawTriangles(vertices.data(), colors.data(), vertexCount);
    }

    void VkCanvas::drawSimpleRect(float left, float top, float width, float height, const glm::vec4 color){
        Cmd cmd;
        cmd.type = CmdType::DrawSimpleRect;
        DrawSimpleRectData data{.left = left, .top = top, .width = width , .height = height,.color = color};
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
        Cmd cmd;
        cmd.type = CmdType::DrawSprite;

        DrawSpriteData data;
        data.image = image;
        data.srcRect = srcRect;
        data.dstRect = dstRect;
        cmd.data = data;
        
        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::flush(){
    }
}


