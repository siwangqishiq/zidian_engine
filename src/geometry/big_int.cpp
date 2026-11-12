#include "geometry/big_int.h"

#include <algorithm>
#include <stdexcept>
#include <ostream>
#include <limits>

namespace zidian {
    BigInt::BigInt() = default;

    BigInt::BigInt(long long value) {
        if (value == 0) {
            return;
        }

        sign_ = value < 0 ? -1 : 1;

        // 避免 -LLONG_MIN 溢出
        unsigned long long magnitude;

        if (value < 0) {
            magnitude = static_cast<unsigned long long>(-(value + 1)) + 1;
        } else {
            magnitude = static_cast<unsigned long long>(value);
        }

        while (magnitude > 0) {
            digits_.push_back(
                static_cast<uint32_t>(magnitude % BASE)
            );
            magnitude /= BASE;
        }

        normalize();
    }

    BigInt::BigInt(const std::string& value) {
        if (value.empty()) {
            throw std::invalid_argument("BigInt: empty string");
        }

        size_t pos = 0;

        if (value[pos] == '+' || value[pos] == '-') {
            sign_ = value[pos] == '-' ? -1 : 1;
            ++pos;
        } else {
            sign_ = 1;
        }

        if (pos == value.size()) {
            throw std::invalid_argument("BigInt: invalid number");
        }

        for (size_t i = pos; i < value.size(); ++i) {
            if (value[i] < '0' || value[i] > '9') {
                throw std::invalid_argument("BigInt: invalid character");
            }
        }

        // 从字符串末尾按 9 位分组
        for (size_t end = value.size(); end > pos;) {
            size_t begin = end >= pos + BASE_DIGITS
                ? end - BASE_DIGITS
                : pos;

            uint32_t part = 0;

            for (size_t i = begin; i < end; ++i) {
                part = part * 10 + static_cast<uint32_t>(value[i] - '0');
            }

            digits_.push_back(part);
            end = begin;
        }

        normalize();
    }

    void BigInt::normalize() {
        while (!digits_.empty() && digits_.back() == 0) {
            digits_.pop_back();
        }

        if (digits_.empty()) {
            sign_ = 0;
        } else if (sign_ == 0) {
            sign_ = 1;
        }
    }

    int BigInt::sign() const {
        return sign_;
    }

    bool BigInt::isZero() const {
        return sign_ == 0;
    }

    bool BigInt::isPositive() const {
        return sign_ > 0;
    }

    bool BigInt::isNegative() const {
        return sign_ < 0;
    }

    BigInt BigInt::abs() const {
        BigInt result = *this;

        if (!result.isZero()) {
            result.sign_ = 1;
        }

        return result;
    }

    std::string BigInt::toString() const {
        if (isZero()) {
            return "0";
        }

        std::string result;

        if (sign_ < 0) {
            result.push_back('-');
        }

        result += std::to_string(digits_.back());

        for (size_t i = digits_.size() - 1; i > 0; --i) {
            std::string part = std::to_string(digits_[i - 1]);

            result.append(BASE_DIGITS - part.size(), '0');
            result += part;
        }

        return result;
    }

    int BigInt::compareAbs(const BigInt& lhs, const BigInt& rhs) {
        if (lhs.digits_.size() != rhs.digits_.size()) {
            return lhs.digits_.size() < rhs.digits_.size() ? -1 : 1;
        }

        for (size_t i = lhs.digits_.size(); i > 0; --i) {
            if (lhs.digits_[i - 1] != rhs.digits_[i - 1]) {
                return lhs.digits_[i - 1] < rhs.digits_[i - 1]
                    ? -1
                    : 1;
            }
        }

        return 0;
    }

    BigInt BigInt::addAbs(const BigInt& lhs, const BigInt& rhs) {
        BigInt result;
        result.sign_ = 1;

        size_t n = std::max(lhs.digits_.size(), rhs.digits_.size());

        result.digits_.resize(n);

        uint64_t carry = 0;

        for (size_t i = 0; i < n; ++i) {
            uint64_t sum = carry;

            if (i < lhs.digits_.size()) {
                sum += lhs.digits_[i];
            }

            if (i < rhs.digits_.size()) {
                sum += rhs.digits_[i];
            }

            result.digits_[i] = static_cast<uint32_t>(sum % BASE);
            carry = sum / BASE;
        }

        if (carry > 0) {
            result.digits_.push_back(static_cast<uint32_t>(carry));
        }

        result.normalize();
        return result;
    }

    // 前提：|lhs| >= |rhs|
    BigInt BigInt::subtractAbs(const BigInt& lhs, const BigInt& rhs) {
        BigInt result;
        result.sign_ = 1;
        result.digits_.resize(lhs.digits_.size());

        int64_t borrow = 0;

        for (size_t i = 0; i < lhs.digits_.size(); ++i) {
            int64_t value = static_cast<int64_t>(lhs.digits_[i]) - borrow;

            if (i < rhs.digits_.size()) {
                value -= rhs.digits_[i];
            }

            if (value < 0) {
                value += BASE;
                borrow = 1;
            } else {
                borrow = 0;
            }

            result.digits_[i] = static_cast<uint32_t>(value);
        }

        result.normalize();
        return result;
    }

    BigInt BigInt::multiplyAbsSmall(
        const BigInt& value,
        uint32_t multiplier
    ) {
        if (value.isZero() || multiplier == 0) {
            return BigInt();
        }

        BigInt result;
        result.sign_ = 1;
        result.digits_.resize(value.digits_.size());

        uint64_t carry = 0;

        for (size_t i = 0; i < value.digits_.size(); ++i) {
            uint64_t current =
                static_cast<uint64_t>(value.digits_[i]) * multiplier
                + carry;

            result.digits_[i] = static_cast<uint32_t>(current % BASE);
            carry = current / BASE;
        }

        if (carry > 0) {
            result.digits_.push_back(static_cast<uint32_t>(carry));
        }

        result.normalize();
        return result;
    }

    BigInt BigInt::multiplyAbs(const BigInt& lhs, const BigInt& rhs) {
        if (lhs.isZero() || rhs.isZero()) {
            return BigInt();
        }

        BigInt result;
        result.sign_ = 1;
        result.digits_.assign(
            lhs.digits_.size() + rhs.digits_.size(),
            0
        );

        for (size_t i = 0; i < lhs.digits_.size(); ++i) {
            uint64_t carry = 0;

            for (size_t j = 0; j < rhs.digits_.size(); ++j) {
                uint64_t current =
                    result.digits_[i + j]
                    + static_cast<uint64_t>(lhs.digits_[i]) * rhs.digits_[j]
                    + carry;

                result.digits_[i + j] =
                    static_cast<uint32_t>(current % BASE);

                carry = current / BASE;
            }

            size_t pos = i + rhs.digits_.size();

            while (carry > 0) {
                if (pos == result.digits_.size()) {
                    result.digits_.push_back(0);
                }

                uint64_t current = result.digits_[pos] + carry;

                result.digits_[pos] =
                    static_cast<uint32_t>(current % BASE);

                carry = current / BASE;
                ++pos;
            }
        }

        result.normalize();
        return result;
    }

    void BigInt::shiftBaseAdd(uint32_t digit) {
        if (isZero()) {
            if (digit != 0) {
                sign_ = 1;
                digits_.push_back(digit);
            }
            return;
        }

        digits_.insert(digits_.begin(), digit);
        normalize();
    }

    BigInt BigInt::operator-() const {
        BigInt result = *this;
        result.sign_ = -result.sign_;
        return result;
    }

    BigInt& BigInt::operator+=(const BigInt& rhs) {
        if (rhs.isZero()) {
            return *this;
        }

        if (isZero()) {
            *this = rhs;
            return *this;
        }

        if (sign_ == rhs.sign_) {
            BigInt result = addAbs(*this, rhs);
            result.sign_ = sign_;
            *this = std::move(result);
            return *this;
        }

        int cmp = compareAbs(*this, rhs);

        if (cmp == 0) {
            *this = BigInt();
        } else if (cmp > 0) {
            BigInt result = subtractAbs(*this, rhs);
            result.sign_ = sign_;
            *this = std::move(result);
        } else {
            BigInt result = subtractAbs(rhs, *this);
            result.sign_ = rhs.sign_;
            *this = std::move(result);
        }

        return *this;
    }

    BigInt& BigInt::operator-=(const BigInt& rhs) {
        return *this += -rhs;
    }

    BigInt& BigInt::operator*=(const BigInt& rhs) {
        int resultSign = sign_ * rhs.sign_;

        BigInt result = multiplyAbs(*this, rhs);

        if (!result.isZero()) {
            result.sign_ = resultSign;
        }

        *this = std::move(result);
        return *this;
    }

    BigInt& BigInt::operator/=(const BigInt& rhs) {
        *this = divmod(*this, rhs).first;
        return *this;
    }

    BigInt& BigInt::operator%=(const BigInt& rhs) {
        *this = divmod(*this, rhs).second;
        return *this;
    }

    BigInt operator+(const BigInt& lhs, const BigInt& rhs) {
        BigInt result = lhs;
        result += rhs;
        return result;
    }

    BigInt operator-(const BigInt& lhs, const BigInt& rhs) {
        BigInt result = lhs;
        result -= rhs;
        return result;
    }

    BigInt operator*(const BigInt& lhs, const BigInt& rhs) {
        BigInt result = lhs;
        result *= rhs;
        return result;
    }

    BigInt operator/(const BigInt& lhs, const BigInt& rhs) {
        return BigInt::divmod(lhs, rhs).first;
    }

    BigInt operator%(const BigInt& lhs, const BigInt& rhs) {
        return BigInt::divmod(lhs, rhs).second;
    }

    bool operator==(const BigInt& lhs, const BigInt& rhs) {
        return lhs.sign_ == rhs.sign_ && lhs.digits_ == rhs.digits_;
    }

    bool operator!=(const BigInt& lhs, const BigInt& rhs) {
        return !(lhs == rhs);
    }

    bool operator<(const BigInt& lhs, const BigInt& rhs) {
        if (lhs.sign_ != rhs.sign_) {
            return lhs.sign_ < rhs.sign_;
        }

        if (lhs.sign_ == 0) {
            return false;
        }

        int cmp = BigInt::compareAbs(lhs, rhs);

        return lhs.sign_ > 0 ? cmp < 0 : cmp > 0;
    }

    bool operator<=(const BigInt& lhs, const BigInt& rhs) {
        return !(rhs < lhs);
    }

    bool operator>(const BigInt& lhs, const BigInt& rhs) {
        return rhs < lhs;
    }

    bool operator>=(const BigInt& lhs, const BigInt& rhs) {
        return !(lhs < rhs);
    }

    std::pair<BigInt, BigInt> BigInt::divmodAbs(
        const BigInt& lhs,
        const BigInt& rhs
    ) {
        if (rhs.isZero()) {
            throw std::domain_error("BigInt division by zero");
        }

        BigInt dividend = lhs.abs();
        BigInt divisor = rhs.abs();

        if (compareAbs(dividend, divisor) < 0) {
            return {BigInt(), dividend};
        }

        BigInt quotient;
        quotient.sign_ = 1;
        quotient.digits_.assign(dividend.digits_.size(), 0);

        BigInt remainder;

        for (size_t i = dividend.digits_.size(); i > 0; --i) {
            remainder.shiftBaseAdd(dividend.digits_[i - 1]);

            // 二分搜索当前位商，范围 [0, BASE)
            uint64_t low = 0;
            uint64_t high = BASE;

            while (low < high) {
                uint64_t mid = low + (high - low) / 2;

                BigInt product = multiplyAbsSmall(
                    divisor,
                    static_cast<uint32_t>(mid)
                );

                if (compareAbs(product, remainder) <= 0) {
                    low = mid + 1;
                } else {
                    high = mid;
                }
            }

            uint32_t digit = static_cast<uint32_t>(low - 1);

            quotient.digits_[i - 1] = digit;

            if (digit > 0) {
                BigInt product = multiplyAbsSmall(divisor, digit);
                remainder = subtractAbs(remainder, product);
            }
        }

        quotient.normalize();
        remainder.normalize();

        return {quotient, remainder};
    }

    std::pair<BigInt, BigInt> BigInt::divmod(
        const BigInt& lhs,
        const BigInt& rhs
    ) {
        if (rhs.isZero()) {
            throw std::domain_error("BigInt division by zero");
        }

        auto result = divmodAbs(lhs, rhs);

        if (!result.first.isZero()) {
            result.first.sign_ = lhs.sign_ * rhs.sign_;
        }

        if (!result.second.isZero()) {
            result.second.sign_ = lhs.sign_;
        }

        return result;
    }

    BigInt BigInt::gcd(BigInt lhs, BigInt rhs) {
        lhs = lhs.abs();
        rhs = rhs.abs();

        while (!rhs.isZero()) {
            BigInt remainder = lhs % rhs;
            lhs = std::move(rhs);
            rhs = std::move(remainder);
        }

        return lhs;
    }

    BigInt BigInt::integerSqrt(const BigInt& value) {
        if (value.isNegative()) {
            throw std::domain_error("BigInt integerSqrt of negative value");
        }

        if (value.isZero()) {
            return BigInt();
        }

        // 初始值：10^ceil(decimal_digits / 2)
        size_t decimalDigits = value.toString().size();

        BigInt x(
            "1" + std::string((decimalDigits + 1) / 2, '0')
        );

        const BigInt two(2);

        while (true) {
            BigInt y = (x + value / x) / two;

            if (y >= x) {
                return x;
            }

            x = std::move(y);
        }
    }

    std::ostream& operator<<(std::ostream& os, const BigInt& value) {
        return os << value.toString();
    }

}

