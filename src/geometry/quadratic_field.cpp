#include "geometry/quadratic_field.h"
#include <stdexcept>
#include <cmath>
#include <limits>

namespace zidian {
    QuadraticField::QuadraticField(const Rational& d) : d_(d) {
        if (d_.sign() <= 0) {
            throw std::invalid_argument("d must be positive");
        }

        if (isRationalSquare(d_)) {
            throw std::invalid_argument("d must not be a rational square");
        }
    }

    const Rational& QuadraticField::getD() const {
        return d_;
    }

    bool QuadraticField::operator==(const QuadraticField& other) const {
        return d_ == other.d_;
    }

    bool QuadraticField::operator!=(const QuadraticField& other) const {
        return !(*this == other);
    }

    bool QuadraticField::isRationalSquare(const Rational& value) {
        const Integer numerator = value.getNumerator();
        const Integer denominator = value.getDenominator();

        return isPerfectSquare(numerator) && isPerfectSquare(denominator);
    }

    bool QuadraticField::isPerfectSquare(Integer value) {
        if (value < 0) {
            return false;
        }

        const Integer root = static_cast<Integer>(
            std::sqrt(static_cast<long double>(value))
        );

        // 使用除法验证，避免 root * root 溢出。
        return root == 0
            ? value == 0
            : (value / root == root && value % root == 0)
            || ((root + 1) <= value / (root + 1)
                && (root + 1) * (root + 1) == value);
    }
}