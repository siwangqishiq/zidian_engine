#include "renderer/render.h"
#include "renderer/vk_canvas.h"
#include "renderer/vertex/primitive_vertex.h"


namespace zidian {
    VkCanvas::VkCanvas(Render &context) : ctx(context) {
    }

    VkCanvas::~VkCanvas(){
    }

    void VkCanvas::drawTriangles(const glm::vec2 *vertices,const glm::vec4 *colors,const uint32_t vertexCount){
        uint32_t startVertex = ctx.commandList.getPrimitiveVertices().size();

        PrimitiveVertex vert[vertexCount];
        for(int i = 0; i < vertexCount ; i++){
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
        const uint32_t vertexCount = 6;
        const glm::vec4 colors[vertexCount] = {color,color,color,color,color,color};
        const glm::vec2 vertices[vertexCount] = {
            {left + width, top},
            {left, top},
            {left , top + height},
            {left + width, top},
            {left , top + height},
            {left + width, top + height}
        };

        drawTriangles(vertices, colors, vertexCount);
    }

    void VkCanvas::flush(){

    }
}


