#pragma once

#include "geometry/rational.h"
#include <vector>
#include <string>

namespace zidian {

    class QuadraticField;

    class QuadraticNumber{
    public:
        // 在指定数域中创建零
        explicit QuadraticNumber(const QuadraticField& field);

        // 在指定数域中创建一个数
        QuadraticNumber(
            const QuadraticField& field,
            const Rational& value
        );

        // 使用完整系数数组创建
        QuadraticNumber(
            const QuadraticField& field,
            const std::vector<Rational>& coefficients
        );

        QuadraticNumber(
            const QuadraticField& field,
            const Rational& a,
            const Rational& b
        );

        //获取数域
        const QuadraticField& getField() const;

        //获取系数数组列表
        const std::vector<Rational>& getCoefficients() const;

        bool isZero() const;
        bool isPositive() const;
        bool isNegative() const;

        int sign() const;

        // 共轭与范数
        QuadraticNumber conjugate() const;
        QuadraticNumber norm() const;

        double toDouble() const;
        std::string toString() const;

        QuadraticNumber operator-() const;

        QuadraticNumber& operator+=(const QuadraticNumber& rhs);
        QuadraticNumber& operator-=(const QuadraticNumber& rhs);
        QuadraticNumber& operator*=(const QuadraticNumber& rhs);
        QuadraticNumber& operator/=(const QuadraticNumber& rhs);

        friend QuadraticNumber operator+(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend QuadraticNumber operator-(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend QuadraticNumber operator*(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend QuadraticNumber operator/(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend bool operator==(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend bool operator!=(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend bool operator<(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend bool operator<=(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend bool operator>(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );

        friend bool operator>=(
            const QuadraticNumber& lhs,
            const QuadraticNumber& rhs
        );
    private:
        const QuadraticField& field_;
        std::vector<Rational> coefficients_;
    };
}