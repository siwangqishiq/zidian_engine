#include "geometry/rational.h"

#include <limits>
#include <stdexcept>
#include <ostream>
#include <numeric>

namespace zidian {
    using UnsignedInteger = unsigned long long;

     UnsignedInteger magnitude(Integer value) {
        if (value >= 0) {
            return static_cast<UnsignedInteger>(value);
        }

        return static_cast<UnsignedInteger>(-(value + 1)) + 1;
    }

    int comparePositiveFractions(unsigned long long a,unsigned long long b,
        unsigned long long c,unsigned long long d) {
        bool reversed = false;
        while (true) {
            auto q1 = a / b;
            auto q2 = c / d;

            if (q1 != q2) {
                bool less = q1 < q2;

                if (reversed) {
                    less = !less;
                }

                return less ? -1 : 1;
            }

            auto r1 = a % b;
            auto r2 = c % d;

            if (r1 == 0 || r2 == 0) {
                if (r1 == 0 && r2 == 0) {
                    return 0;
                }

                bool less = (r1 == 0);

                if (reversed) {
                    less = !less;
                }

                return less ? -1 : 1;
            }

            // 继续比较倒数后的余数分数
            a = b;
            b = r1;

            c = d;
            d = r2;

            reversed = !reversed;
        }//end while
        return 0;
    }

    //检测数字加法是否溢出
    Integer checkedAdd(Integer a, Integer b) {
        Integer result;
        if (__builtin_add_overflow(a, b, &result)) {
            throw std::overflow_error("Rational integer overflow");
        }
        return result;
    }

    //检测数字减法是否溢出
    Integer checkedSub(Integer a, Integer b) {
        Integer result;
        if (__builtin_sub_overflow(a, b, &result)) {
            throw std::overflow_error("Rational integer overflow");
        }
        return result;
    }

    Integer checkedMul(Integer a, Integer b) {
        Integer result;
        if (__builtin_mul_overflow(a, b, &result)) {
            throw std::overflow_error("Rational integer overflow");
        }
        return result;
    }

    Integer checkedNegate(Integer value) {
        if (value == std::numeric_limits<Integer>::min()) {
            throw std::overflow_error("Rational integer overflow");
        }
        return -value;
    }

    Rational::Rational() : numerator_(0), denominator_(1) {
    }

    Rational::Rational(Integer value) : numerator_(value), denominator_(1) {
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

    bool Rational::isZero() const {
        return numerator_ == 0;
    }

    bool Rational::isPositive() const {
        return numerator_ > 0;
    }

    bool Rational::isNegative() const {
        return numerator_ < 0;
    }

    int Rational::sign() const {
        if (numerator_ > 0) {
            return 1;
        }

        if (numerator_ < 0) {
            return -1;
        }
        return 0;
    }

    double Rational::toDouble() const {
        return static_cast<double>(numerator_) /
            static_cast<double>(denominator_);
    }

    std::string Rational::toString() const {
        if (denominator_ == 1) {
            return std::to_string(numerator_);
        }

        return std::to_string(numerator_) + "/" +
            std::to_string(denominator_);
    }

    Rational Rational::operator-() const {
        return Rational(checkedNegate(numerator_), denominator_);
    }

    Rational& Rational::operator+=(const Rational& rhs) {
        // 先约分两个分母，降低中间结果的大小
        Integer g = std::gcd(denominator_, rhs.denominator_);

        Integer leftFactor = rhs.denominator_ / g;
        Integer rightFactor = denominator_ / g;

        Integer n = checkedAdd(
            checkedMul(numerator_, leftFactor),
            checkedMul(rhs.numerator_, rightFactor)
        );

        Integer d = checkedMul(denominator_, leftFactor);

        numerator_ = n;
        denominator_ = d;

        normalize();
        return *this;
    }

    Rational& Rational::operator-=(const Rational& rhs) {
        return *this += -rhs;
    }

    Rational& Rational::operator*=(const Rational& rhs) {
        // 交叉约分
        Integer g1 = static_cast<Integer>(
            gcd(magnitude(numerator_), magnitude(rhs.denominator_))
        );

        Integer g2 = static_cast<Integer>(
            gcd(magnitude(rhs.numerator_), magnitude(denominator_))
        );

        Integer a = numerator_ / g1;
        Integer b = rhs.numerator_ / g2;
        Integer c = denominator_ / g2;
        Integer d = rhs.denominator_ / g1;

        numerator_ = checkedMul(a, b);
        denominator_ = checkedMul(c, d);

        normalize();
        return *this;
    }

    Rational& Rational::operator/=(const Rational& rhs) {
        if (rhs.numerator_ == 0) {
            throw std::domain_error("Rational: division by zero");
        }

        Integer rhsNumerator = rhs.numerator_;
        Integer rhsDenominator = rhs.denominator_;

        // 交叉约分
        Integer g1 = std::gcd(numerator_, rhsNumerator);
        Integer g2 = std::gcd(rhsDenominator, denominator_);

        Integer a = numerator_ / g1;
        Integer b = rhsDenominator / g2;
        Integer c = denominator_ / g2;
        Integer d = rhsNumerator / g1;

        numerator_ = checkedMul(a, b);
        denominator_ = checkedMul(c, d);

        normalize();
        return *this;
    }

    Integer Rational::getNumerator() const{
        return numerator_;
    }

    Integer Rational::getDenominator() const{
        return denominator_;
    }

    bool operator==(const Rational& lhs, const Rational& rhs) {
        return lhs.numerator_ == rhs.numerator_ && lhs.denominator_ == rhs.denominator_;
    }

    bool operator!=(const Rational& lhs, const Rational& rhs) {
        return !(lhs == rhs);
    }

    bool operator<(const Rational& lhs, const Rational& rhs) {
        int leftSign = lhs.sign();
        int rightSign = rhs.sign();

        if (leftSign != rightSign) {
            return leftSign < rightSign;
        }

        if (leftSign == 0) {
            return false;
        }

        int result = comparePositiveFractions(
            magnitude(lhs.numerator_),
            static_cast<unsigned long long>(lhs.denominator_),
            magnitude(rhs.numerator_),
            static_cast<unsigned long long>(rhs.denominator_)
        );

        // 两个负数比较大小时，绝对值较大的反而较小
        return leftSign > 0 ? result < 0 : result > 0;
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

    std::ostream& operator<<(std::ostream& os, const Rational& value) {
        return os << value.toString();
    }

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

    UnsignedInteger Rational::gcd(UnsignedInteger a,UnsignedInteger b) {
        while (b != 0) {
            unsigned long long remainder = a % b;
            a = b;
            b = remainder;
        }

        return a;
    }

    void Rational::normalize() {
        if (denominator_ == 0) {
            throw std::invalid_argument(
                "Rational: denominator cannot be zero"
            );
        }

        // 零优先处理，避免对 LLONG_MIN 分母取负
        if (numerator_ == 0) {
            denominator_ = 1;
            return;
        }

        bool negative = (numerator_ < 0) != (denominator_ < 0);

        unsigned long long n = magnitude(numerator_);
        unsigned long long d = magnitude(denominator_);

        unsigned long long divisor = gcd(n, d);

        n /= divisor;
        d /= divisor;

        const unsigned long long maxValue =
            static_cast<unsigned long long>(
                std::numeric_limits<Integer>::max()
            );

        // 分母必须能用正的 long long 表示
        if (d > maxValue) {
            throw std::overflow_error(
                "Rational denominator overflow"
            );
        }

        // 负数允许表示 LLONG_MIN
        if (negative) {
            if (n == maxValue + 1) {
                numerator_ = std::numeric_limits<Integer>::min();
            } else {
                numerator_ = -static_cast<Integer>(n);
            }
        } else {
            // 正数不能超过 LLONG_MAX
            if (n > maxValue) {
                throw std::overflow_error(
                    "Rational numerator overflow"
                );
            }

            numerator_ = static_cast<Integer>(n);
        }

        denominator_ = static_cast<Integer>(d);
    }

}