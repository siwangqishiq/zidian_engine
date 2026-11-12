
#include "geometry/rational.h"

#include <stdexcept>
#include <ostream>
#include <cmath>
#include <string>

namespace zidian {

    namespace {

        // 将 BigInt 转换成科学计数法形式的 long double。
        // 避免直接转换超大整数时超出 long double 的表示范围。
        long double toScientificValue(const BigInt& value, int& exponent) {
            std::string str = value.abs().toString();

            if (str == "0") {
                exponent = 0;
                return 0.0L;
            }

            constexpr size_t PRECISION = 18;

            size_t count = str.size() < PRECISION
                ? str.size()
                : PRECISION;

            long double mantissa = 0.0L;

            for (size_t i = 0; i < count; ++i) {
                mantissa = mantissa * 10.0L
                         + static_cast<long double>(str[i] - '0');
            }

            exponent = static_cast<int>(str.size() - count);

            if (value.isNegative()) {
                mantissa = -mantissa;
            }

            return mantissa;
        }

    }

    Rational::Rational()
        : numerator_(0), denominator_(1) {
    }

    Rational::Rational(Integer value)
        : numerator_(value), denominator_(1) {
    }

    Rational::Rational(Integer numerator, Integer denominator)
        : numerator_(numerator), denominator_(denominator) {
        normalize();
    }

    Integer Rational::numerator() const {
        return numerator_;
    }

    Integer Rational::denominator() const {
        return denominator_;
    }

    Integer Rational::getNumerator() const {
        return numerator_;
    }

    Integer Rational::getDenominator() const {
        return denominator_;
    }

    bool Rational::isZero() const {
        return numerator_.isZero();
    }

    bool Rational::isPositive() const {
        return numerator_.isPositive();
    }

    bool Rational::isNegative() const {
        return numerator_.isNegative();
    }

    int Rational::sign() const {
        return numerator_.sign();
    }

    double Rational::toDouble() const {
        if (numerator_.isZero()) {
            return 0.0;
        }

        int numeratorExponent = 0;
        int denominatorExponent = 0;

        long double numeratorValue =
            toScientificValue(numerator_, numeratorExponent);

        long double denominatorValue =
            toScientificValue(denominator_, denominatorExponent);

        int exponent = numeratorExponent - denominatorExponent;

        long double result = numeratorValue / denominatorValue;

        result *= std::pow(10.0L, static_cast<long double>(exponent));

        return static_cast<double>(result);
    }

    std::string Rational::toString() const {
        if (denominator_ == Integer(1)) {
            return numerator_.toString();
        }

        return numerator_.toString() + "/" +
               denominator_.toString();
    }

    Rational Rational::operator-() const {
        return Rational(-numerator_, denominator_);
    }

    // 加法
    Rational& Rational::operator+=(const Rational& rhs) {
        // 先约分两个分母，降低中间结果的大小
        Integer g = gcd(denominator_, rhs.denominator_);

        Integer leftFactor = rhs.denominator_ / g;
        Integer rightFactor = denominator_ / g;

        Integer n = numerator_ * leftFactor
                  + rhs.numerator_ * rightFactor;

        Integer d = denominator_ * leftFactor;

        numerator_ = n;
        denominator_ = d;

        normalize();
        return *this;
    }

    // 减法
    Rational& Rational::operator-=(const Rational& rhs) {
        return *this += (-rhs);
    }

    // 乘法
    Rational& Rational::operator*=(const Rational& rhs) {
        // 交叉约分
        Integer g1 = gcd(numerator_, rhs.denominator_);
        Integer g2 = gcd(rhs.numerator_, denominator_);

        Integer a = numerator_ / g1;
        Integer b = rhs.numerator_ / g2;
        Integer c = denominator_ / g2;
        Integer d = rhs.denominator_ / g1;

        numerator_ = a * b;
        denominator_ = c * d;

        normalize();
        return *this;
    }

    // 除法
    Rational& Rational::operator/=(const Rational& rhs) {
        if (rhs.numerator_.isZero()) {
            throw std::domain_error("Rational: division by zero");
        }

        // 交叉约分
        Integer g1 = gcd(numerator_, rhs.numerator_);
        Integer g2 = gcd(rhs.denominator_, denominator_);

        Integer a = numerator_ / g1;
        Integer b = rhs.denominator_ / g2;
        Integer c = denominator_ / g2;
        Integer d = rhs.numerator_ / g1;

        numerator_ = a * b;
        denominator_ = c * d;

        normalize();
        return *this;
    }

    // 相等
    bool operator==(const Rational& lhs, const Rational& rhs) {
        return lhs.numerator_ == rhs.numerator_
            && lhs.denominator_ == rhs.denominator_;
    }

    bool operator!=(const Rational& lhs, const Rational& rhs) {
        return !(lhs == rhs);
    }

    // 小于
    bool operator<(const Rational& lhs, const Rational& rhs) {
        return lhs.numerator_ * rhs.denominator_
             < rhs.numerator_ * lhs.denominator_;
    }

    bool operator<=(const Rational& lhs, const Rational& rhs) {
        return !(rhs < lhs);
    }

    bool operator>(const Rational& lhs, const Rational& rhs) {
        return rhs < lhs;
    }

    bool operator>=(const Rational& lhs, const Rational& rhs) {
        return !(lhs < rhs);
    }

    // 输出
    std::ostream& operator<<(std::ostream& os, const Rational& value) {
        return os << value.toString();
    }

    // 四则运算
    Rational operator+(Rational lhs, const Rational& rhs) {
        lhs += rhs;
        return lhs;
    }

    Rational operator-(Rational lhs, const Rational& rhs) {
        lhs -= rhs;
        return lhs;
    }

    Rational operator*(Rational lhs, const Rational& rhs) {
        lhs *= rhs;
        return lhs;
    }

    Rational operator/(Rational lhs, const Rational& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // 最大公约数
    Integer Rational::gcd(Integer a, Integer b) {
        return BigInt::gcd(a.abs(), b.abs());
    }

    // 规范化分数
    void Rational::normalize() {
        if (denominator_.isZero()) {
            throw std::invalid_argument(
                "Rational: denominator cannot be zero"
            );
        }

        // 零统一表示为 0/1
        if (numerator_.isZero()) {
            denominator_ = Integer(1);
            return;
        }

        // 保证分母为正数
        if (denominator_.isNegative()) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }

        // 最大公约数约分
        Integer divisor = gcd(numerator_, denominator_);

        numerator_ /= divisor;
        denominator_ /= divisor;
    }

}
