#pragma once

#include <variant>
#include "renderer/command/data/data_simple_rect.h"
#include "renderer/command/data/data_simple_circle.h"
#include "renderer/command/data/data_simple_triangle.h"
#include "renderer/command/data/data_triangles.h"
#include "renderer/command/data/data_sprite.h"

namespace zidian{
    enum class CmdType{
        DrawSimpleRect,
        DrawSimpleCircle,
        DrawSimpleTriangle,
        DrawTriangles,
        DrawSprite
    };

    using CmdDataType = std::variant<
        DrawSimpleRectData,
        DrawSimpleCircleData,
        DrawSimpleTriangleData,
        DrawTrianglesData,
        DrawSpriteData
    >;

    struct Cmd{
        CmdType type;
        CmdDataType data;
    };
}