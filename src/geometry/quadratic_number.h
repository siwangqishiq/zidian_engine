#pragma once

#include "geometry/quadratic_field.h"

namespace zidian {
    class  QuadraticNumber{
    public:
        QuadraticNumber(
            const QuadraticField& field,
            const Rational& a,
            const Rational& b
        );

        const QuadraticField& getField() const;
        const Rational& getA() const;
        const Rational& getB() const;

        // 加减乘除
        QuadraticNumber& operator+=(const QuadraticNumber& other);
        QuadraticNumber& operator-=(const QuadraticNumber& other);
        QuadraticNumber& operator*=(const QuadraticNumber& other);
        QuadraticNumber& operator/=(const QuadraticNumber& other);

        // 比较
        bool operator==(const QuadraticNumber& other) const;
        bool operator!=(const QuadraticNumber& other) const;

        int sign() const;
        bool isZero() const;
        bool isPositive() const;
        bool isNegative() const;


        bool operator<(const QuadraticNumber& other) const;
        bool operator<=(const QuadraticNumber& other) const;
        bool operator>(const QuadraticNumber& other) const;
        bool operator>=(const QuadraticNumber& other) const;

        // 共轭：a - b√d
        QuadraticNumber conjugate() const;

        // 范数：a² - b²d
        Rational norm() const;

    private:
        const QuadraticField& field_;

        Rational a_;
        Rational b_;

        void checkSameField(const QuadraticNumber& other) const;
    };

    QuadraticNumber operator+(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    );

    QuadraticNumber operator-(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    );

    QuadraticNumber operator*(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    );

    QuadraticNumber operator/(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    );
}