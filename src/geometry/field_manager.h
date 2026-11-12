#pragma once

#include "geometry/quadratic_field.h"
#include "geometry/quadratic_number.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

namespace zidian {

    class FieldManager {
    public:

        struct SqrtResult {
            const QuadraticField& field;
            QuadraticNumber value;
            bool extended;

            SqrtResult(
                const QuadraticField& field,
                const QuadraticNumber& value,
                bool extended
            );
        };

    public:
        FieldManager();
        ~FieldManager();

        FieldManager(const FieldManager&) = delete;
        FieldManager& operator=(const FieldManager&) = delete;

        FieldManager(FieldManager&&) = delete;
        FieldManager& operator=(FieldManager&&) = delete;

        const QuadraticField& getRoot() const;

        // 创建二次扩张。
        // 如果存在等价扩张，则复用已有数域。
        const QuadraticField& extend(
            const QuadraticField& parent,
            const QuadraticNumber& radicand
        );

        // 仅在当前数域中求平方根。
        bool trySqrt(
            const QuadraticField& field,
            const QuadraticNumber& value,
            QuadraticNumber& result
        ) const;

        // 尝试在当前数域求根，必要时自动扩张。
        // 负数返回 std::nullopt。
        std::optional<SqrtResult> trySqrtOrExtend(
            const QuadraticField& field,
            const QuadraticNumber& value
        );

        std::size_t getFieldCount() const;

    private:

        struct ExtensionNode;

        bool ownsField(const QuadraticField& field) const;

        // 查找与指定扩张域等价的已有扩张。
        // 不要求两个扩张具有相同的父域。
        const ExtensionNode* findEquivalentExtension(
            const QuadraticField& parent,
            const QuadraticNumber& radicand
        ) const;

        // 在已有扩张域中构造对应 radicand 的平方根。
        // radicand 可以属于另一个等价父域。
        QuadraticNumber createExtendedSqrt(
            const ExtensionNode& node,
            const QuadraticField& sourceParent,
            const QuadraticNumber& radicand
        ) const;

    private:

        QuadraticField root_;

        std::vector<std::unique_ptr<ExtensionNode>> extensions_;
    };

}