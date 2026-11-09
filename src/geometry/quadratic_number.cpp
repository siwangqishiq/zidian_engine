#include "geometry/quadratic_number.h"

#include <stdexcept>

namespace zidian {
    QuadraticNumber::QuadraticNumber(const QuadraticField& field,const Rational& a,const Rational& b)
        : field_(field), a_(a), b_(b) {
    }


    const QuadraticField& QuadraticNumber::getField() const {
        return field_;
    }

    const Rational& QuadraticNumber::getA() const {
        return a_;
    }

    const Rational& QuadraticNumber::getB() const {
        return b_;
    }

    void QuadraticNumber::checkSameField(const QuadraticNumber& other) const {
        if (field_ != other.field_) {
            throw std::invalid_argument("QuadraticNumber: different fields");
        }
    }

    // 加法
    QuadraticNumber& QuadraticNumber::operator+=(const QuadraticNumber& other) {
        checkSameField(other);

        a_ += other.a_;
        b_ += other.b_;

        return *this;
    }

    // 减法
    QuadraticNumber& QuadraticNumber::operator-=(const QuadraticNumber& other) {
        checkSameField(other);

        a_ -= other.a_;
        b_ -= other.b_;

        return *this;
    }

    // 乘法
    QuadraticNumber& QuadraticNumber::operator*=(const QuadraticNumber& other) {
        checkSameField(other);

        const Rational& d = field_.getD();

        Rational a = a_ * other.a_ + b_ * other.b_ * d;
        Rational b = a_ * other.b_ + b_ * other.a_;

        a_ = a;
        b_ = b;

        return *this;
    }

    // 除法
    QuadraticNumber& QuadraticNumber::operator/=(const QuadraticNumber& other) {
        checkSameField(other);

        const Rational& d = field_.getD();

        // 分母：c² - e²d
        Rational denominator =
            other.a_ * other.a_
            - other.b_ * other.b_ * d;

        if (denominator.isZero()) {
            throw std::domain_error("Division by zero");
        }

        Rational a =
            (a_ * other.a_ - b_ * other.b_ * d)
            / denominator;

        Rational b =
            (b_ * other.a_ - a_ * other.b_)
            / denominator;

        a_ = a;
        b_ = b;

        return *this;
    }

    // 相等比较
    bool QuadraticNumber::operator==(const QuadraticNumber& other) const {
        checkSameField(other);
        return a_ == other.a_ && b_ == other.b_;
    }

    bool QuadraticNumber::operator!=(const QuadraticNumber& other) const {
        return !(*this == other);
    }

    int QuadraticNumber::sign() const{
         // a + b√d
        const int signA = a_.sign();
        const int signB = b_.sign();

        // a = 0
        if (signA == 0) {
            return signB;
        }

        // b = 0
        if (signB == 0) {
            return signA;
        }

        // a、b 同号
        if (signA == signB) {
            return signA;
        }

        // a、b 异号，比较 a² 和 b²d
        const Rational lhs = a_ * a_;
        const Rational rhs = b_ * b_ * field_.getD();

        const int comparison = (lhs - rhs).sign();

        if (comparison == 0) {
            return 0;
        }

        // a² > b²d，结果符号与 a 相同
        if (comparison > 0) {
            return signA;
        }

        // a² < b²d，结果符号与 b 相同
        return signB;
    }
        
    bool QuadraticNumber::isZero() const{
        return sign() == 0;
    }
        
    bool QuadraticNumber::isPositive() const{
        return sign() > 0;
    }

    bool QuadraticNumber::isNegative() const{
        return sign() < 0;
    }

    // 普通加法
    QuadraticNumber operator+(const QuadraticNumber& lhs,const QuadraticNumber& rhs) {
        QuadraticNumber result = lhs;
        result += rhs;
        return result;
    }

    // 普通减法
    QuadraticNumber operator-(const QuadraticNumber& lhs,const QuadraticNumber& rhs) {
        QuadraticNumber result = lhs;
        result -= rhs;
        return result;
    }

    // 普通乘法
    QuadraticNumber operator*(const QuadraticNumber& lhs,const QuadraticNumber& rhs) {
        QuadraticNumber result = lhs;
        result *= rhs;
        return result;
    }

    // 普通除法
    QuadraticNumber operator/(const QuadraticNumber& lhs,const QuadraticNumber& rhs) {
        QuadraticNumber result = lhs;
        result /= rhs;
        return result;
    }
}