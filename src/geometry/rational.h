#pragma once

#include <string>
#include <iosfwd>
#include "geometry/big_int.h"

namespace zidian {
    using Integer = BigInt; 

    class Rational{
    public:
        Rational();
        Rational(Integer value);
        Rational(Integer numerator, Integer denominator);

        Integer numerator() const;
        Integer denominator() const;

        bool isZero() const;
        bool isPositive() const;
        bool isNegative() const;
        int sign() const;

        double toDouble() const;
        std::string toString() const;

        Rational operator-() const;

        Rational& operator+=(const Rational& rhs);
        Rational& operator-=(const Rational& rhs);
        Rational& operator*=(const Rational& rhs);
        Rational& operator/=(const Rational& rhs);

        friend bool operator==(const Rational& lhs, const Rational& rhs);
        friend bool operator!=(const Rational& lhs, const Rational& rhs);
        friend bool operator<(const Rational& lhs, const Rational& rhs);
        friend bool operator<=(const Rational& lhs, const Rational& rhs);
        friend bool operator>(const Rational& lhs, const Rational& rhs);
        friend bool operator>=(const Rational& lhs, const Rational& rhs);

        friend std::ostream& operator<<(std::ostream& os, const Rational& value);

        Integer getNumerator() const;
        Integer getDenominator() const;

    private:
        Integer numerator_; //分子
        Integer denominator_; //分母

        static Integer gcd(Integer a, Integer b);
        void normalize();
    };

    Rational operator+(Rational lhs, const Rational& rhs);
    Rational operator-(Rational lhs, const Rational& rhs);
    Rational operator*(Rational lhs, const Rational& rhs);
    Rational operator/(Rational lhs, const Rational& rhs);
}