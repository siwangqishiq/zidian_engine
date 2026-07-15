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

    void VkCanvas::flush(){

    }
}


