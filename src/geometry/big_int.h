
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <iosfwd>

namespace zidian {
    class BigInt {
    public:
        BigInt();
        BigInt(long long value);
        explicit BigInt(const std::string& value);

        std::string toString() const;

        int sign() const;
        bool isZero() const;
        bool isPositive() const;
        bool isNegative() const;

        BigInt abs() const;

        // 返回 floor(sqrt(value))，负数抛出异常
        static BigInt integerSqrt(const BigInt& value);

        // 最大公约数，结果非负
        static BigInt gcd(BigInt lhs, BigInt rhs);

        // 商和余数：商向零截断，余数符号与 lhs 相同
        static std::pair<BigInt, BigInt> divmod(
            const BigInt& lhs,
            const BigInt& rhs
        );

        BigInt operator-() const;

        BigInt& operator+=(const BigInt& rhs);
        BigInt& operator-=(const BigInt& rhs);
        BigInt& operator*=(const BigInt& rhs);
        BigInt& operator/=(const BigInt& rhs);
        BigInt& operator%=(const BigInt& rhs);

        friend BigInt operator+(const BigInt& lhs, const BigInt& rhs);
        friend BigInt operator-(const BigInt& lhs, const BigInt& rhs);
        friend BigInt operator*(const BigInt& lhs, const BigInt& rhs);
        friend BigInt operator/(const BigInt& lhs, const BigInt& rhs);
        friend BigInt operator%(const BigInt& lhs, const BigInt& rhs);

        friend bool operator==(const BigInt& lhs, const BigInt& rhs);
        friend bool operator!=(const BigInt& lhs, const BigInt& rhs);
        friend bool operator<(const BigInt& lhs, const BigInt& rhs);
        friend bool operator<=(const BigInt& lhs, const BigInt& rhs);
        friend bool operator>(const BigInt& lhs, const BigInt& rhs);
        friend bool operator>=(const BigInt& lhs, const BigInt& rhs);

        friend std::ostream& operator<<(std::ostream& os, const BigInt& value);

    private:
        static constexpr uint32_t BASE = 1000000000;
        static constexpr int BASE_DIGITS = 9;

        // sign_: -1、0、1
        int sign_ = 0;

        // 小端存储，每个元素范围 [0, BASE)
        std::vector<uint32_t> digits_;

        void normalize();

        static int compareAbs(const BigInt& lhs, const BigInt& rhs);

        static BigInt addAbs(const BigInt& lhs, const BigInt& rhs);
        static BigInt subtractAbs(const BigInt& lhs, const BigInt& rhs);
        static BigInt multiplyAbs(const BigInt& lhs, const BigInt& rhs);

        static BigInt multiplyAbsSmall(
            const BigInt& value,
            uint32_t multiplier
        );

        // value = value * BASE + digit
        void shiftBaseAdd(uint32_t digit);

        static std::pair<BigInt, BigInt> divmodAbs(
            const BigInt& lhs,
            const BigInt& rhs
        );
    };

}
