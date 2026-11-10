#pragma once

#include "geometry/rational.h"
#include <vector>
#include <cstddef>

namespace zidian{
    class QuadraticNumber;

    class QuadraticField {
    public:
        // 有理数域 Q
        QuadraticField();

         // 创建一个新的二次扩张
        QuadraticField(
            const QuadraticField& parent,
            const QuadraticNumber& radicand
        );

        ~QuadraticField() = default;

         // 禁止复制和移动，保证父数域引用稳定
        QuadraticField(const QuadraticField&) = delete;
        QuadraticField& operator=(const QuadraticField&) = delete;

        QuadraticField(QuadraticField&&) = delete;
        QuadraticField& operator=(QuadraticField&&) = delete;

        // 是否为有理数域
        bool isBaseField() const;

        // 获取扩张层数
        size_t getLevel() const;

        // 获取数域维度
        size_t getDimension() const;

        // 获取父数域
        const QuadraticField* getParent() const;

        // 获取本层被开方数
        const QuadraticNumber* getRadicand() const;

    private:
        const QuadraticField* parent_ = nullptr;
        // 
        const QuadraticNumber* radicand_ = nullptr;

        size_t level_ = 0;
        size_t dimension_ = 1;
    };
}