#include "geometry/quadratic_field.h"
#include "geometry/quadratic_number.h"
#include <limits>
#include <stdexcept>

namespace zidian {
    QuadraticField::QuadraticField() = default;

    QuadraticField::QuadraticField(
        const QuadraticField& parent,
        const QuadraticNumber& radicand
    ): parent_(&parent),radicand_(&radicand) {
        if (&radicand.getField() != &parent) {
            throw std::invalid_argument(
                "Radicand must belong to parent field"
            );
        }

        level_ = parent.getLevel() + 1;

        // 检查维度翻倍是否溢出
        if (parent.getDimension() > std::numeric_limits<size_t>::max() / 2) {
            throw std::overflow_error(
                "Quadratic field dimension overflow"
            );
        }

        dimension_ = parent.getDimension() * 2;
    }

    bool QuadraticField::isBaseField() const {
        return parent_ == nullptr;
    }

    size_t QuadraticField::getLevel() const {
        return level_;
    }

    size_t QuadraticField::getDimension() const {
        return dimension_;
    }

    const QuadraticField* QuadraticField::getParent() const {
        return parent_;
    }

    const QuadraticNumber* QuadraticField::getRadicand() const {
        return radicand_;
    }
}