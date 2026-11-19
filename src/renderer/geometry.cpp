#include "renderer/geometry.h"

namespace zidian {
    float Rect::right() const{
        return left + width;
    }

    float Rect::bottom() const{
        return top + height;
    }

    std::vector<glm::vec2> Geometry::genRectVertices(float left, float top, float width, float height) const{
        std::vector<glm::vec2> vertices = {
            {left + width, top},
            {left, top},
            {left , top + height},
            {left + width, top},
            {left , top + height},
            {left + width, top + height}
        };
        return vertices;
    }

    std::vector<glm::vec2> Geometry::genCircleVertices(float cx, float cy, float radius, uint32_t step) const{
        std::vector<glm::vec2> vertices;
        if (step < 3 || radius <= 0.0f) {
            return vertices;
        }

        vertices.reserve(step * 3);
        float delta = 2.0f * PI / static_cast<float>(step);
        
        glm::vec2 center(cx, cy);
        for (uint32_t i = 0; i < step; i++){
            float angle0 = i * delta;
            float angle1 = (i + 1) * delta;

            vertices.emplace_back(cx, cy);
            vertices.emplace_back(
                cx + cos(angle0) * radius,
                cy + sin(angle0) * radius
            );

            vertices.emplace_back(
                cx + cos(angle1) * radius,
                cy + sin(angle1) * radius
            );
        }//end for i
        return vertices;
    }

    std::array<glm::vec2, 2> Geometry::buildAABB(glm::vec2 &pA , glm::vec2 &pB, glm::vec2 &pC){
        float minX = pA[0];
        if(pB[0] < minX){
            minX = pB[0];
        }
        if(pC[0] < minX){
            minX = pC[0];
        }

        float minY = pA[1];
        if(pB[1] < minY){
            minY = pB[1];
        }
        if(pC[1] < minY){
            minY = pC[1];
        }

        float maxX = pA[0];
        if(pB[0] > maxX){
            maxX = pB[0];
        }
        if(pC[0] > maxX){
            maxX = pC[0];
        }

        float maxY = pA[1];
        if(pB[1] > maxY){
            maxY = pB[1];
        }
        if(pC[1] > maxY){
            maxY = pC[1];
        }
        
        return {glm::vec2{minX, minY},glm::vec2{maxX, maxY}};
    }


    std::vector<glm::vec2> Geometry::genLinesVertices(glm::vec2 *points, uint32_t pointCount, float lineWidth) const{
        std::vector<glm::vec2> vertices;
        if (points == nullptr || pointCount < 2 || lineWidth <= 0.0f) {
            return vertices;
        }

        // 去除连续重复点，避免产生零长度线段
        std::vector<glm::vec2> validPoints;
        validPoints.reserve(pointCount);

        constexpr float EPSILON = 1e-6f;

        for (uint32_t i = 0; i < pointCount; ++i) {
            if (validPoints.empty() ||
                glm::length(points[i] - validPoints.back()) > EPSILON) {
                validPoints.push_back(points[i]);
            }
        }//end for i

        const uint32_t count = static_cast<uint32_t>(validPoints.size());

        if (count < 2) {
            return vertices;
        }

        const float halfWidth = lineWidth * 0.5f;

        // 每条线段的单位法线
        std::vector<glm::vec2> normals(count - 1);

        for (uint32_t i = 0; i < count - 1; ++i) {
            glm::vec2 direction = glm::normalize(
                validPoints[i + 1] - validPoints[i]
            );

            normals[i] = glm::vec2(-direction.y, direction.x);
        }

        // 每个路径点对应的宽度偏移量
        std::vector<glm::vec2> offsets(count);

        // 起点
        offsets[0] = normals[0] * halfWidth;

        // 中间连接点：Miter Join
        constexpr float MITER_LIMIT = 4.0f;

        for (uint32_t i = 1; i < count - 1; ++i) {
            glm::vec2 miter = normals[i - 1] + normals[i];

            float len = glm::length(miter);

            if (len < EPSILON) {
                offsets[i] = normals[i] * halfWidth;
                continue;
            }

            miter /= len;

            float denom = glm::dot(miter, normals[i]);

            if (std::abs(denom) < EPSILON) {
                offsets[i] = normals[i] * halfWidth;
                continue;
            }

            glm::vec2 offset = miter * (halfWidth / denom);

            // 限制尖锐角度下的 Miter 长度
            float maxLength = halfWidth * MITER_LIMIT;
            float offsetLength = glm::length(offset);

            if (offsetLength > maxLength) {
                offset *= maxLength / offsetLength;
            }

            offsets[i] = offset;
        }

        // 终点
        offsets[count - 1] = normals[count - 2] * halfWidth;

        // 每条线段生成两个三角形
        vertices.reserve((count - 1) * 6);

        for (uint32_t i = 0; i < count - 1; ++i) {
            glm::vec2 p0 = validPoints[i];
            glm::vec2 p1 = validPoints[i + 1];

            glm::vec2 n0 = offsets[i];
            glm::vec2 n1 = offsets[i + 1];

            glm::vec2 v0 = p0 + n0;
            glm::vec2 v1 = p0 - n0;
            glm::vec2 v2 = p1 + n1;
            glm::vec2 v3 = p1 - n1;

            // Triangle 1
            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v2);

            // Triangle 2
            vertices.push_back(v2);
            vertices.push_back(v1);
            vertices.push_back(v3);
        }

        return vertices;
    }

    void Geometry::splitBezier2(
            const Bezier2& curve,
            Bezier2& left,
            Bezier2& right){
        auto a = (curve.p0 + curve.p1) * 0.5f;
        auto b = (curve.p1 + curve.p2) * 0.5f;
        auto m = (a + b) * 0.5f;

        left = {curve.p0, a, m};
        right = {m, b, curve.p2};
    }

    void Geometry::splitBezier3(const Bezier3& curve, Bezier3& left,Bezier3& right){
        auto a = (curve.p0 + curve.p1) * 0.5f;
        auto b = (curve.p1 + curve.p2) * 0.5f;
        auto c = (curve.p2 + curve.p3) * 0.5f;

        auto d = (a + b) * 0.5f;
        auto e = (b + c) * 0.5f;

        auto m = (d + e) * 0.5f;

        left = {curve.p0, a, d, m};
        right = {m, e, c, curve.p3};
    }

    float Geometry::distanceToLine(glm::vec2 p,glm::vec2 a,glm::vec2 b) const{
        auto ab = b - a;
        float len = glm::length(ab);

        if (len < 1e-6f) {
            return glm::length(p - a);
        }

        return std::abs(
            ab.x * (p.y - a.y) -
            ab.y * (p.x - a.x)
        ) / len;
    }

    bool Geometry::isFlatEnough(const Bezier3& curve,float tolerance) const{
        float d1 = distanceToLine(
            curve.p1, curve.p0, curve.p3
        );

        float d2 = distanceToLine(
            curve.p2, curve.p0, curve.p3
        );

        return std::max(d1, d2) <= tolerance;
    }

    bool Geometry::isFlatEnough(const Bezier2& curve,float tolerance) const{
        return distanceToLine(curve.p1,curve.p0,curve.p2) <= tolerance;
    }

    void Geometry::flattenBezier2(
            const Bezier2& curve,
            float tolerance,
            std::vector<glm::vec2>& output,
            int depth
        ){
        if (depth >= 16 || isFlatEnough(curve, tolerance)) {
            output.push_back(curve.p2);
            return;
        }

        Bezier2 left, right;
        splitBezier2(curve, left, right);

        flattenBezier2(left, tolerance, output, depth + 1);
        flattenBezier2(right, tolerance, output, depth + 1);
    }

    void Geometry::flattenBezier3(
            const Bezier3& curve,
            float tolerance,
            std::vector<glm::vec2>& output,
            int depth)
    {
        if (depth >= 16 || isFlatEnough(curve, tolerance)) {
            output.push_back(curve.p3);
            return;
        }

        Bezier3 left, right;
        splitBezier3(curve, left, right);

        flattenBezier3(left, tolerance, output, depth + 1);
        flattenBezier3(right, tolerance, output, depth + 1);
    }

     std::vector<glm::vec2> Geometry::genBezier3Vertices(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2,glm::vec2 p3) {
        std::vector<glm::vec2> points;
        points.push_back(p0);
        
        const Bezier3 curve {p0, p1, p2, p3};
        flattenBezier3(curve,0.5f,points);
        return points;
    }

    std::vector<glm::vec2> Geometry::genBezier2Vertices(glm::vec2 p0,glm::vec2 p1,glm::vec2 p2){
        std::vector<glm::vec2> points;
        points.push_back(p0);

        const Bezier2 curve {p0, p1, p2};
        flattenBezier2(curve,0.5f,points);
        return points;
    }
}