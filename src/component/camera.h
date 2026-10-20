#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace zidian{
    class Camera2D{
    public:
        glm::vec2 position{0.0f, 0.0f};// 相机中心的世界坐标
        
        glm::vec2 viewportSize{1280.0f, 720.0f};// 视口尺寸，单位为逻辑像素
        
        float zoom = 1.0f;// 缩放倍率：2 表示放大两倍
       
        float rotation = 0.0f; // 相机旋转角度，单位为弧度

        glm::mat4 view() const;

        glm::mat4 projection() const;

        glm::mat4 viewProjection() const;

        glm::mat4 inverseViewProjection() const;

        glm::vec2 screenToWorld(glm::vec2 screenPos) const;
    };
}