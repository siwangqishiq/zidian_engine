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

    void VkCanvas::drawTriangles(const glm::vec2 *vertices,const glm::vec4 *colors,const uint32_t vertexCount){
        uint32_t startVertex = ctx.commandList.getPrimitiveVertices().size();

        PrimitiveVertex vert[vertexCount];
        for(int i = 0; i < vertexCount; i++){
            vert[i].position = glm::vec3(vertices[i][0], vertices[i][1], 0.0f);
            vert[i].color = colors[i];
            ctx.commandList.getPrimitiveVertices().push_back(vert[i]);
        }//end for i

        PrimitiveCommand command;
        command.firstVertex = startVertex;
        command.vertexCount = vertexCount;
        ctx.commandList.getPrimitiveCommands().push_back(command);

        
    }

    void VkCanvas::drawTriangle(const glm::vec2 *vertices,const glm::vec4 color){
        const uint32_t vertexCount = 3;
        const glm::vec4 colors[vertexCount] = {color,color,color};
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
        const glm::vec4 colors[vertexCount] = {color,color,color,color,color,color};
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
        cmd.rectData.color = color;
        cmd.rectData.left = left;
        cmd.rectData.top = top;
        cmd.rectData.width = width;
        cmd.rectData.height = height;
        
        ctx.renderQueue->addCmd(cmd);
    }
    
    void VkCanvas::drawSimpleTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4 color){
        Cmd cmd;
        cmd.type = CmdType::DrawSimpleTriangle;
        cmd.triangleData.color = color;
        cmd.triangleData.p1 = p1;
        cmd.triangleData.p2 = p2;
        cmd.triangleData.p3 = p3;
        
        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::drawSimpleCircle(glm::vec2 center, float radius, const glm::vec4 color){
        Cmd cmd;
        cmd.type = CmdType::DrawSimpleCircle;
        cmd.circleData.color = color;
        cmd.circleData.center = center;
        cmd.circleData.radius = radius;
        
        ctx.renderQueue->addCmd(cmd);
    }

    void VkCanvas::drawImage(Image &image, Rect &srcRect, Rect &dstRect){
        //todo 
    }

    void VkCanvas::flush(){
    }
}


