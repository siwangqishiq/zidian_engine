#pragma once

#include "renderer/command/data/data_simple_rect.h"
#include "renderer/command/data/data_simple_circle.h"
#include "renderer/command/data/data_simple_triangle.h"

namespace zidian{
    enum class CmdType{
        DrawSimpleRect,
        DrawSimpleCircle,
        DrawSimpleTriangle
    };

    struct Cmd{
        CmdType type;
        
        union {
            DrawSimpleRectData rectData;
            DrawSimpleCircleData circleData;
            DrawSimpleTriangleData triangleData;
        };
    };
}