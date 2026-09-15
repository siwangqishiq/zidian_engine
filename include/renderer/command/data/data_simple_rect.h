#pragma once

#include "glm/glm.hpp"

namespace zidian{
    struct DrawSimpleRectData{
        float left;
        float top;
        float width;
        float height;
        
        glm::vec4 color;
    };
}