
#pragma once

#include "geometry/quadratic_field.h"
#include "geometry/quadratic_number.h"

#include <unordered_map>

namespace zidian {

    class FieldEmbedding {
    public:
        // 建立从 source 到 target 的实数域嵌入。
        // 如果无法建立，抛出 std::invalid_argument。
        FieldEmbedding(
            const QuadraticField& source,
            const QuadraticField& target
        );

        const QuadraticField& getSource() const;
        const QuadraticField& getTarget() const;

        // 将源域中的元素转换到目标域。
        QuadraticNumber convert(
            const QuadraticNumber& value
        ) const;

    private:
        const QuadraticField& source_;
        const QuadraticField& target_;

        // 缓存每个源扩张节点的生成元在目标域中的映射。
        //
        // key:
        //     源扩张节点
        //
        // value:
        //     该节点的 sqrt(radicand) 在目标域中的表示
        //
        // 每个 FieldEmbedding 实例拥有独立缓存。
        mutable std::unordered_map<
            const QuadraticField*,
            QuadraticNumber
        > mappedGenerators_;

        // 验证源域是否能够嵌入目标域。
        void validateEmbedding(
            const QuadraticField& source
        ) const;

        // 获取扩张节点生成元在目标域中的映射。
        // 如果尚未缓存，则递归计算并缓存。
        const QuadraticNumber& getMappedGenerator(
            const QuadraticField& source
        ) const;

        // 递归转换元素。
        QuadraticNumber convertRecursive(
            const QuadraticNumber& value
        ) const;
    };

}
