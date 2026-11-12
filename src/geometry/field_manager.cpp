#include "geometry/field_manager.h"
#include "geometry/field_embedding.h"

#include <stdexcept>
#include <utility>


namespace zidian {

    // ==================================================
    // SqrtResult
    // ==================================================

    FieldManager::SqrtResult::SqrtResult(
        const QuadraticField& field,
        const QuadraticNumber& value,
        bool extended
    )
        : field(field),
          value(value),
          extended(extended) {
    }

    // ==================================================
    // 扩张节点
    // ==================================================

    struct FieldManager::ExtensionNode {

        QuadraticNumber radicand;
        QuadraticField field;

        ExtensionNode(
            const QuadraticField& parent,
            const QuadraticNumber& value
        )
            : radicand(value),
              field(parent, radicand) {
        }
    };

    // ==================================================
    // 构造与析构
    // ==================================================

    FieldManager::FieldManager() = default;

    FieldManager::~FieldManager() {

        // 子节点晚于父节点创建，因此逆序销毁。
        while (!extensions_.empty()) {
            extensions_.pop_back();
        }
    }

    // ==================================================
    // 根域
    // ==================================================

    const QuadraticField& FieldManager::getRoot() const {
        return root_;
    }

    // ==================================================
    // 判断数域归属
    // ==================================================

    bool FieldManager::ownsField(
        const QuadraticField& field
    ) const {

        if (&field == &root_) {
            return true;
        }

        for (const auto& node : extensions_) {

            if (&node->field == &field) {
                return true;
            }
        }

        return false;
    }

    // ==================================================
    // 查找等价扩张
    //
    // 不再要求：
    //
    // node->field.getParent() == &parent
    //
    // 而是检查：
    //
    // candidate <-> existing
    //
    // 是否能够建立双向实数域嵌入。
    // ==================================================

    const FieldManager::ExtensionNode*
    FieldManager::findEquivalentExtension(
        const QuadraticField& parent,
        const QuadraticNumber& radicand
    ) const {

        // 构造本次请求对应的临时扩张。
        //
        // 这里只用于验证，不会加入 extensions_。
        //
        // candidate 的 radicand 引用在本函数执行期间有效。
        QuadraticField candidate(parent, radicand);

        const std::size_t candidateDimension =
            candidate.getDimension();

        for (const auto& node : extensions_) {

            const QuadraticField& existing =
                node->field;

            // 有限扩张如果存在双向嵌入，其维度必须相同。
            if (existing.getDimension() != candidateDimension) {
                continue;
            }

            try {

                // candidate -> existing
                FieldEmbedding forward(
                    candidate,
                    existing
                );

                // existing -> candidate
                FieldEmbedding backward(
                    existing,
                    candidate
                );

                // 两个方向均成功，认定为等价扩张。
                return node.get();
            }
            catch (const std::invalid_argument&) {

                // 当前候选域与这个已有域不等价。
                // 继续检查下一个节点。
            }
        }

        return nullptr;
    }

    // ==================================================
    // 创建扩张域中的平方根
    //
    // 已有扩张：
    //
    // L = K(sqrt(b))
    //
    // 当前请求：
    //
    // sqrt(a)，其中 a 属于 K'
    //
    // 首先建立：
    //
    // phi: K' -> K
    //
    // 然后：
    //
    // a' = phi(a)
    //
    // c = sqrt(a' / b)
    //
    // sqrt(a') = c * sqrt(b)
    // ==================================================

    QuadraticNumber FieldManager::createExtendedSqrt(
        const ExtensionNode& node,
        const QuadraticField& sourceParent,
        const QuadraticNumber& radicand
    ) const {

        // 完整的已有扩张域：
        //
        // L = K(sqrt(b))
        //
        const QuadraticField& targetField =
            node.field;

        // --------------------------------------------------
        // 将源父域嵌入完整目标域。
        //
        // sourceParent -> targetField
        // --------------------------------------------------

        FieldEmbedding embedding(
            sourceParent,
            targetField
        );

        // 将 radicand 转换到目标域。
        QuadraticNumber mappedRadicand =
            embedding.convert(radicand);

        // --------------------------------------------------
        // 直接在完整目标域中求平方根。
        // --------------------------------------------------

        QuadraticNumber result(targetField);

        if (!mappedRadicand.trySqrt(result)) {
            throw std::logic_error(
                "FieldManager: equivalent extension "
                "does not contain the required square root"
            );
        }

        return result;
    }

    // ==================================================
    // 创建二次扩张
    // ==================================================

    const QuadraticField& FieldManager::extend(
        const QuadraticField& parent,
        const QuadraticNumber& radicand
    ) {

        if (!ownsField(parent)) {
            throw std::invalid_argument(
                "FieldManager: parent field is not managed"
            );
        }

        if (&radicand.getField() != &parent) {
            throw std::invalid_argument(
                "FieldManager: radicand belongs to another field"
            );
        }

        if (radicand.sign() <= 0) {
            throw std::invalid_argument(
                "FieldManager: radicand must be positive"
            );
        }

        // radicand 已经是当前域中的平方，不需要扩张。
        QuadraticNumber sqrtValue(parent);

        if (radicand.trySqrt(sqrtValue)) {
            throw std::invalid_argument(
                "FieldManager: radicand is already a square in parent field"
            );
        }

        // 检查是否存在等价扩张。
        const ExtensionNode* existing =
            findEquivalentExtension(parent, radicand);

        if (existing != nullptr) {
            return existing->field;
        }

        // 创建新扩张。
        auto node = std::make_unique<ExtensionNode>(
            parent,
            radicand
        );

        const QuadraticField& result =
            node->field;

        extensions_.push_back(std::move(node));

        return result;
    }

    // ==================================================
    // 当前数域中求平方根
    // ==================================================

    bool FieldManager::trySqrt(
        const QuadraticField& field,
        const QuadraticNumber& value,
        QuadraticNumber& result
    ) const {

        if (!ownsField(field)) {
            throw std::invalid_argument(
                "FieldManager: field is not managed"
            );
        }

        if (&value.getField() != &field) {
            throw std::invalid_argument(
                "FieldManager: value belongs to another field"
            );
        }

        if (&result.getField() != &field) {
            throw std::invalid_argument(
                "FieldManager: result belongs to another field"
            );
        }

        return value.trySqrt(result);
    }

    // ==================================================
    // 自动求平方根与扩张
    // ==================================================

    std::optional<FieldManager::SqrtResult>
    FieldManager::trySqrtOrExtend(
        const QuadraticField& field,
        const QuadraticNumber& value
    ) {

        if (!ownsField(field)) {
            throw std::invalid_argument(
                "FieldManager: field is not managed"
            );
        }

        if (&value.getField() != &field) {
            throw std::invalid_argument(
                "FieldManager: value belongs to another field"
            );
        }

        // --------------------------------------------------
        // 1. 优先尝试当前域内求根。
        // --------------------------------------------------

        QuadraticNumber sqrtValue(field);

        if (value.trySqrt(sqrtValue)) {

            return std::optional<SqrtResult>(
                std::in_place,
                field,
                sqrtValue,
                false
            );
        }

        // --------------------------------------------------
        // 2. 负数无法在实数域中开平方。
        // --------------------------------------------------

        if (value.sign() < 0) {
            return std::nullopt;
        }

        // --------------------------------------------------
        // 3. 查找已有的等价扩张。
        // --------------------------------------------------

        const ExtensionNode* existing =
            findEquivalentExtension(field, value);

        if (existing != nullptr) {

            QuadraticNumber result =
                createExtendedSqrt(
                    *existing,
                    field,
                    value
                );

            return std::optional<SqrtResult>(
                std::in_place,
                existing->field,
                result,
                false
            );
        }

        // --------------------------------------------------
        // 4. 创建新的二次扩张。
        // --------------------------------------------------

        const QuadraticField& extendedField =
            extend(field, value);

        // 如果 extend() 返回的是已有域，那么上面的查找
        // 理论上已经处理了这种情况。
        //
        // 这里创建的是刚创建扩张时的根基向量。

        std::vector<Rational> coefficients(
            extendedField.getDimension(),
            Rational(0)
        );

        const std::size_t sqrtIndex =
            field.getDimension();

        coefficients[sqrtIndex] =
            Rational(1);

        QuadraticNumber extendedRoot(
            extendedField,
            coefficients
        );

        return std::optional<SqrtResult>(
            std::in_place,
            extendedField,
            extendedRoot,
            true
        );
    }

    // ==================================================
    // 数域数量
    // ==================================================

    std::size_t FieldManager::getFieldCount() const {
        return extensions_.size() + 1;
    }

}