#include "component/camera.h"

namespace zidian{
    glm::mat4 Camera2D::view() const{
        glm::mat4 v(1.0f);
        v = glm::rotate(v, -rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        v = glm::translate(v, glm::vec3(-position, 0.0f));
        return v;
    }

    glm::mat4 Camera2D::projection() const{
        float halfW = viewportSize.x / (2.0f * zoom);
        float halfH = viewportSize.y / (2.0f * zoom);

        // Vulkan 深度范围 [0, 1]
        glm::mat4 p = glm::orthoRH_ZO(
            -halfW, halfW,
            -halfH, halfH,
            -1.0f, 1.0f
        );

        // 约定世界坐标 Y 轴向上，且使用正高度 Vulkan viewport
        p[1][1] *= -1.0f;
        return p;
    }

    glm::mat4 Camera2D::viewProjection() const {
        return projection() * view();
    }

    glm::mat4 Camera2D::inverseViewProjection() const {
        return glm::inverse(viewProjection());
    }

    glm::vec2 Camera2D::screenToWorld(glm::vec2 screenPos) const {
        // 屏幕像素坐标 -> NDC
        float x = 2.0f * screenPos.x / viewportSize.x - 1.0f;
        float y = 1.0f - 2.0f * screenPos.y / viewportSize.y;
        
        // NDC -> 世界坐标
        glm::vec4 world = inverseViewProjection() * glm::vec4(x, y, 0.5f, 1.0f);
        return glm::vec2(world) / world.w;
    }
}