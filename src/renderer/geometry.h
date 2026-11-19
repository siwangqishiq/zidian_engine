#pragma once

#include <vector>
#include "glm/glm.hpp"
#include <array>


namespace zidian{
    constexpr float PI = 3.14159265358979323846f;

    struct Rect{
        float left;
        float top;
        float width;
        float height;

        float right() const;
        float bottom() const;

        Rect(float l,float t, float w, float h) : left(l),top(t), width(w),height(h){}
        Rect() : left(0.0f),top(0.0f), width(0.0f),height(0.0f){}
    };

    struct Bezier2 {
        glm::vec2 p0;
        glm::vec2 p1;
        glm::vec2 p2;
    };

    struct Bezier3 {
        glm::vec2 p0;
        glm::vec2 p1;
        glm::vec2 p2;
        glm::vec2 p3;
    };

    class Geometry{
    public:
        std::vector<glm::vec2> genRectVertices(float left, float top, float width, float height) const;

        std::vector<glm::vec2> genCircleVertices(float cx, float cy, float raidus, uint32_t step = 180) const;

        std::array<glm::vec2, 2> buildAABB(glm::vec2 &pA , glm::vec2 &pB, glm::vec2 &pC);

        std::vector<glm::vec2> genLinesVertices(glm::vec2 *points, uint32_t pointCount, float lineWidth) const;

        std::vector<glm::vec2> genBezier2Vertices(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2);

        std::vector<glm::vec2> genBezier3Vertices(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2,glm::vec2 p3);

        void flattenBezier3(
            const Bezier3& curve,
            float tolerance,
            std::vector<glm::vec2>& output,
            int depth = 0
        );

        void flattenBezier2(
            const Bezier2& curve,
            float tolerance,
            std::vector<glm::vec2>& output,
            int depth = 0
        );

    private:
        void splitBezier3(const Bezier3& curve, Bezier3& left,Bezier3& right);

        void splitBezier2(
            const Bezier2& curve,
            Bezier2& left,
            Bezier2& right
        );

        float distanceToLine(glm::vec2 p,glm::vec2 a,glm::vec2 b) const;
        bool isFlatEnough(const Bezier3& curve,float tolerance) const;
        bool isFlatEnough(const Bezier2& curve,float tolerance) const;
    };

}