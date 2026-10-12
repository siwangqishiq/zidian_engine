#pragma once

#include "renderer/command/data/data_simple_rect.h"
#include "renderer/command/data/data_simple_circle.h"
#include "renderer/command/data/data_simple_triangle.h"
#include "renderer/command/data/data_triangles.h"
#include <variant>

namespace zidian{
    enum class CmdType{
        DrawSimpleRect,
        DrawSimpleCircle,
        DrawSimpleTriangle,
        DrawTriangles
    };

    using CmdDataType = std::variant<
        DrawSimpleRectData,
        DrawSimpleCircleData,
        DrawSimpleTriangleData,
        DrawTrianglesData
    >;

    struct Cmd{
        CmdType type;
        CmdDataType data;
    };
}