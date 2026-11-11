#include "geometry/quadratic_number.h"
#include "geometry/quadratic_field.h"

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <utility>

namespace {
    bool tryIntegerSqrt(
        long long value,
        long long& result
    ) {
        if (value < 0) {
            return false;
        }

        const auto n = static_cast<unsigned long long>(value);

        unsigned long long low = 0;
        unsigned long long high = 4294967296ULL;

        while (low + 1 < high) {
            const unsigned long long mid = low + (high - low) / 2;

            if (mid <= n / mid) {
                low = mid;
            } else {
                high = mid;
            }
        }

        if (low * low != n) {
            return false;
        }

        result = static_cast<long long>(low);
        return true;
    }

}

namespace zidian {
    using Coefficients = std::vector<Rational>;

    //比较两个数阈是否一样 不一样抛出异常
    void checkSameField(const QuadraticField& lhs,const QuadraticField& rhs) {
        if (&lhs != &rhs) {
            throw std::invalid_argument("QuadraticNumber field mismatch");
        }
    }

    void checkDimension(const QuadraticField& field,const Coefficients& coefficients) {
        if (coefficients.size() != field.getDimension()) {
            throw std::invalid_argument("QuadraticNumber coefficient count mismatch");
        }
    }

    //系数加法
    Coefficients add(const Coefficients& lhs,const Coefficients& rhs) {
        Coefficients result(lhs.size());

        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = lhs[i] + rhs[i];
        }

        return result;
    }

    //系数减法
    Coefficients subtract(const Coefficients& lhs,const Coefficients& rhs) {
        Coefficients result(lhs.size());

        for (size_t i = 0; i < lhs.size(); ++i) {
            result[i] = lhs[i] - rhs[i];
        }

        return result;
    }

    //系数求反
    Coefficients negate(const Coefficients& value) {
        Coefficients result(value.size());

        for (size_t i = 0; i < value.size(); ++i) {
            result[i] = -value[i];
        }

        return result;
    }

    //系数乘以一个有理数
    Coefficients multiplyByRational(const Coefficients& value,const Rational& scalar) {
        Coefficients result(value.size());

        for (size_t i = 0; i < value.size(); ++i) {
            result[i] = value[i] * scalar;
        }

        return result;
    }

    bool isZeroCoefficients(const Coefficients& value) {
        for (const auto& coefficient : value) {
            if (!coefficient.isZero()) {
                return false;
            }
        }

        return true;
    }

    QuadraticNumber::QuadraticNumber(const QuadraticField& field)
        : field_(field),coefficients_(field.getDimension(), Rational(0)) {
    }

    QuadraticNumber::QuadraticNumber(
        const QuadraticField& field,
        const Rational& value
    ) : field_(field),coefficients_(field.getDimension(), Rational(0)) {
        coefficients_[0] = value;
    }

    QuadraticNumber::QuadraticNumber(
        const QuadraticField& field,
        const std::vector<Rational>& coefficients
    ) : field_(field),coefficients_(coefficients) {
        checkDimension(field_, coefficients_);
    }

    QuadraticNumber::QuadraticNumber(
        const QuadraticField& field,
        const Rational& a,
        const Rational& b
    ) : field_(field), coefficients_{a, b}
    {
        checkDimension(field_, coefficients_);
    }

    const QuadraticField& QuadraticNumber::getField() const {
        return field_;
    }

    const std::vector<Rational>& QuadraticNumber::getCoefficients() const {
        return coefficients_;
    }

    bool QuadraticNumber::isZero() const {
        return ::zidian::isZeroCoefficients(coefficients_);
    }

    // ============================================================
    // 递归乘法
    //
    // K = K_parent(sqrt(d))
    //
    // x = A + B * sqrt(d)
    // y = C + D * sqrt(d)
    //
    // xy = (AC + dBD) + (AD + BC) * sqrt(d)
    // ============================================================

    Coefficients multiply(
        const QuadraticField& field,
        const Coefficients& lhs,
        const Coefficients& rhs
    ) {
        // 基础数域 Q
        if (field.isBaseField()) {
            return { lhs[0] * rhs[0] };
        }

        const QuadraticField& parent = *field.getParent();

        const size_t half = parent.getDimension();

        // x = A + B * sqrt(d)
        Coefficients a(lhs.begin(), lhs.begin() + half);
        Coefficients b(lhs.begin() + half, lhs.end());

        // y = C + D * sqrt(d)
        Coefficients c(rhs.begin(), rhs.begin() + half);
        Coefficients d(rhs.begin() + half, rhs.end());

        // AC
        Coefficients ac = multiply(parent, a, c);

        // BD
        Coefficients bd = multiply(parent, b, d);

        // AD
        Coefficients ad = multiply(parent, a, d);

        // BC
        Coefficients bc = multiply(parent, b, c);

        // 当前层的 radicand
        const Coefficients& radicand =
            field.getRadicand()->getCoefficients();

        // low = AC + radicand * BD
        Coefficients low = add(
            ac,
            multiply(
                parent,
                radicand,
                bd
            )
        );

        // high = AD + BC
        Coefficients high = add(ad, bc);

        // 重新组合系数
        low.insert(
            low.end(),
            high.begin(),
            high.end()
        );

        return low;
    }

    // ============================================================
    // 递归除法
    //
    // x / y = x * conjugate(y) / norm(y)
    //
    // norm(y) = C² - dD²
    // ============================================================

    Coefficients divide(
        const QuadraticField& field,
        const Coefficients& lhs,
        const Coefficients& rhs
    ) {
        if (isZeroCoefficients(rhs)) {
            throw std::domain_error("Division by zero");
        }

        // 基础数域 Q
        if (field.isBaseField()) {
            return { lhs[0] / rhs[0] };
        }

        const QuadraticField& parent = *field.getParent();

        const size_t half = parent.getDimension();

        // rhs = C + D * sqrt(d)
        Coefficients c(rhs.begin(), rhs.begin() + half);
        Coefficients d(rhs.begin() + half, rhs.end());

        const Coefficients& radicand =
            field.getRadicand()->getCoefficients();

        // norm = C² - radicand * D²
        Coefficients c2 = multiply(parent, c, c);
        Coefficients d2 = multiply(parent, d, d);

        Coefficients norm = subtract(
            c2,
            multiply(parent, radicand, d2)
        );

        if (isZeroCoefficients(norm)) {
            throw std::domain_error("Division by zero");
        }

        // inverse = conjugate(rhs) / norm
        Coefficients inverseLow = divide(
            parent,
            c,
            norm
        );

        Coefficients inverseHigh = negate(
            divide(
                parent,
                d,
                norm
            )
        );

        inverseLow.insert(
            inverseLow.end(),
            inverseHigh.begin(),
            inverseHigh.end()
        );

        return multiply(field, lhs, inverseLow);
    }

    // ============================================================
    // 递归符号判断
    //
    // x = A + B * sqrt(d)
    //
    // A、B 属于父数域，且 d > 0
    // ============================================================

    int signCoefficients(
        const QuadraticField& field,
        const Coefficients& value
    ) {
        // 基础数域 Q
        if (field.isBaseField()) {
            return value[0].sign();
        }

        const QuadraticField& parent = *field.getParent();

        const size_t half = parent.getDimension();

        Coefficients a(value.begin(), value.begin() + half);
        Coefficients b(value.begin() + half, value.end());

        int signA = signCoefficients(parent, a);
        int signB = signCoefficients(parent, b);

        if (signA == 0) {
            return signB;
        }

        if (signB == 0) {
            return signA;
        }

        // A、B 同号
        if (signA == signB) {
            return signA;
        }

        // A、B 异号
        // 比较 A² 和 dB²

        const Coefficients& radicand =
            field.getRadicand()->getCoefficients();

        Coefficients a2 = multiply(parent, a, a);
        Coefficients b2 = multiply(parent, b, b);

        Coefficients difference = subtract(
            a2,
            multiply(parent, radicand, b2)
        );

        int result = signCoefficients(parent, difference);

        return signA > 0 ? result : -result;
    }

    // ============================================================
    // 转换成 double
    // 仅用于渲染和展示，不用于精确判断
    // ============================================================

    double evaluate(
        const QuadraticField& field,
        const Coefficients& value
    ) {
        if (field.isBaseField()) {
            return value[0].toDouble();
        }

        const QuadraticField& parent = *field.getParent();

        const size_t half = parent.getDimension();

        Coefficients a(value.begin(), value.begin() + half);
        Coefficients b(value.begin() + half, value.end());

        double av = evaluate(parent, a);
        double bv = evaluate(parent, b);

        double radicand =
            field.getRadicand()->toDouble();

        return av + bv * std::sqrt(radicand);
    }

    // ============================================================
    // 字符串转换
    // ============================================================

    std::string format(
        const QuadraticField& field,
        const Coefficients& value
    ) {
        if (field.isBaseField()) {
            return value[0].toString();
        }

        const QuadraticField& parent = *field.getParent();

        const size_t half = parent.getDimension();

        Coefficients a(value.begin(), value.begin() + half);
        Coefficients b(value.begin() + half, value.end());

        return "(" + format(parent, a) + ") + (" +
            format(parent, b) + ") * sqrt(" +
            field.getRadicand()->toString() + ")";
    }


    // ============================================================
    // 符号判断
    // ============================================================
    bool QuadraticNumber::isPositive() const {
        return sign() > 0;
    }

    bool QuadraticNumber::isNegative() const {
        return sign() < 0;
    }

    int QuadraticNumber::sign() const {
        return signCoefficients(field_, coefficients_);
    }

    // ============================================================
    // 共轭
    //
    // A + B * sqrt(d)
    // ->
    // A - B * sqrt(d)
    // ============================================================

    QuadraticNumber QuadraticNumber::conjugate() const {
        if (field_.isBaseField()) {
            return *this;
        }

        const size_t half =
            field_.getParent()->getDimension();

        Coefficients result = coefficients_;

        for (size_t i = half; i < result.size(); ++i) {
            result[i] = -result[i];
        }

        return QuadraticNumber(field_, result);
    }

    // ============================================================
    // 范数
    //
    // norm = A² - dB²
    //
    // 返回父数域中的数
    // ============================================================

    QuadraticNumber QuadraticNumber::norm() const {
        if (field_.isBaseField()) {
            return *this;
        }

        const QuadraticField& parent = *field_.getParent();

        const size_t half = parent.getDimension();

        Coefficients a(
            coefficients_.begin(),
            coefficients_.begin() + half
        );

        Coefficients b(
            coefficients_.begin() + half,
            coefficients_.end()
        );

        const Coefficients& radicand =
            field_.getRadicand()->getCoefficients();

        Coefficients a2 = multiply(parent, a, a);
        Coefficients b2 = multiply(parent, b, b);

        Coefficients result = subtract(
            a2,
            multiply(parent, radicand, b2)
        );

        return QuadraticNumber(parent, result);
    }

    // ============================================================
    // 转换
    // ============================================================

    double QuadraticNumber::toDouble() const {
        return evaluate(field_, coefficients_);
    }

    std::string QuadraticNumber::toString() const {
        return format(field_, coefficients_);
    }


    // ============================================================
    // 一元运算
    // ============================================================

    QuadraticNumber QuadraticNumber::operator-() const {
        return QuadraticNumber(
            field_,
            negate(coefficients_)
        );
    }

    // ============================================================
    // 复合赋值
    // ============================================================

    QuadraticNumber& QuadraticNumber::operator+=(
        const QuadraticNumber& rhs
    ) {
        checkSameField(field_, rhs.field_);

        coefficients_ = add(
            coefficients_,
            rhs.coefficients_
        );

        return *this;
    }

    QuadraticNumber& QuadraticNumber::operator-=(
        const QuadraticNumber& rhs
    ) {
        checkSameField(field_, rhs.field_);

        coefficients_ = subtract(
            coefficients_,
            rhs.coefficients_
        );

        return *this;
    }

    QuadraticNumber& QuadraticNumber::operator*=(
        const QuadraticNumber& rhs
    ) {
        checkSameField(field_, rhs.field_);

        coefficients_ = multiply(
            field_,
            coefficients_,
            rhs.coefficients_
        );

        return *this;
    }

    QuadraticNumber& QuadraticNumber::operator/=(
        const QuadraticNumber& rhs
    ) {
        checkSameField(field_, rhs.field_);

        coefficients_ = divide(
            field_,
            coefficients_,
            rhs.coefficients_
        );

        return *this;
    }

    // ============================================================
    // 四则运算
    // ============================================================

    QuadraticNumber operator+(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return QuadraticNumber(
            lhs.field_,
            add(lhs.coefficients_, rhs.coefficients_)
        );
    }

    QuadraticNumber operator-(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return QuadraticNumber(
            lhs.field_,
            subtract(lhs.coefficients_, rhs.coefficients_)
        );
    }

    QuadraticNumber operator*(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return QuadraticNumber(
            lhs.field_,
            multiply(
                lhs.field_,
                lhs.coefficients_,
                rhs.coefficients_
            )
        );
    }

    QuadraticNumber operator/(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return QuadraticNumber(
            lhs.field_,
            divide(
                lhs.field_,
                lhs.coefficients_,
                rhs.coefficients_
            )
        );
    }

    // ============================================================
    // 比较运算
    // ============================================================

    bool operator==(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return lhs.coefficients_ == rhs.coefficients_;
    }

    bool operator!=(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        return !(lhs == rhs);
    }

    bool operator<(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return signCoefficients(
            lhs.field_,
            subtract(lhs.coefficients_, rhs.coefficients_)
        ) < 0;
    }

    bool operator<=(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return !(lhs > rhs);
    }

    bool operator>(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);

        return signCoefficients(
            lhs.field_,
            subtract(lhs.coefficients_, rhs.coefficients_)
        ) > 0;
    }

    bool operator>=(
        const QuadraticNumber& lhs,
        const QuadraticNumber& rhs
    ) {
        checkSameField(lhs.field_, rhs.field_);
        return !(lhs < rhs);
    }

    bool QuadraticNumber::trySqrt(QuadraticNumber& result) const {
        if (&result.getField() != &field_) {
            throw std::invalid_argument("trySqrt: result belongs to a different field");
        }

        if (isNegative()) {
            return false;
        }

        if (isZero()) {
            result.coefficients_.assign(field_.getDimension(), Rational(0));
            return true;
        }

        if (field_.isBaseField()) {
            const Rational& value = coefficients_[0];

            long long numeratorRoot = 0;
            long long denominatorRoot = 0;

            if (!tryIntegerSqrt(value.getNumerator(), numeratorRoot)) {
                return false;
            }

            if (!tryIntegerSqrt(value.getDenominator(), denominatorRoot)) {
                return false;
            }

            result.coefficients_[0] = Rational(
                numeratorRoot,
                denominatorRoot
            );

            return true;
        }

        
         // 当前数域 K = K0(sqrt(d))
        const QuadraticField* parent = field_.getParent();
        const QuadraticNumber* radicand = field_.getRadicand();

        if (parent == nullptr || radicand == nullptr) {
            throw std::logic_error("Invalid quadratic field");
        }

        const size_t parentDimension = parent->getDimension();

        // 拆分 x = A + B * sqrt(d)
        std::vector<Rational> aCoefficients(
            coefficients_.begin(),
            coefficients_.begin() + parentDimension
        );

        std::vector<Rational> bCoefficients(
            coefficients_.begin() + parentDimension,
            coefficients_.end()
        );

        QuadraticNumber A(*parent, aCoefficients);
        QuadraticNumber B(*parent, bCoefficients);

        const QuadraticNumber& d = *radicand;

        // 将父域中的 U、V 组合成当前域中的 U + V * sqrt(d)
        auto compose = [&](const QuadraticNumber& U,
                        const QuadraticNumber& V) {

            std::vector<Rational> coefficients;
            coefficients.reserve(field_.getDimension());

            const auto& u = U.getCoefficients();
            const auto& v = V.getCoefficients();

            coefficients.insert(
                coefficients.end(),
                u.begin(),
                u.end()
            );

            coefficients.insert(
                coefficients.end(),
                v.begin(),
                v.end()
            );

            return QuadraticNumber(field_, coefficients);
        };

        QuadraticNumber zero(*parent);

        // --------------------------------------------------
        // 情况一：B = 0
        // --------------------------------------------------
        if (B.isZero()) {
            // 尝试 x = A，即寻找 U，使 U² = A
            {
                QuadraticNumber U(*parent);

                if (A.trySqrt(U)) {
                    QuadraticNumber candidate = compose(U, zero);

                    if (candidate * candidate == *this) {
                        result.coefficients_ = candidate.coefficients_;
                        return true;
                    }
                }
            }

            // 尝试 x = d * V²，即寻找 V，使 V² = A / d
            {
                QuadraticNumber V(*parent);
                QuadraticNumber target = A / d;

                if (target.trySqrt(V)) {
                    QuadraticNumber candidate = compose(zero, V);

                    if (candidate * candidate == *this) {
                        result.coefficients_ = candidate.coefficients_;
                        return true;
                    }
                }
            }

            return false;
        }

        // --------------------------------------------------
        // 情况二：B != 0
        // --------------------------------------------------

        // N = A² - d * B²
        QuadraticNumber N = A * A - d * B * B;

        // C² = N
        QuadraticNumber C(*parent);

        if (!N.trySqrt(C)) {
            return false;
        }

        const QuadraticNumber two(*parent, Rational(2));
        const QuadraticNumber denominator = two * d;

        // 尝试 C 和 -C 两个分支
        for (int branch = 0; branch < 2; ++branch) {

            QuadraticNumber signedC = branch == 0 ? C : -C;

            // U² = (A + C) / 2
            QuadraticNumber uSquared = (A + signedC) / two;

            // V² = (A - C) / (2d)
            QuadraticNumber vSquared = (A - signedC) / denominator;

            QuadraticNumber U(*parent);
            QuadraticNumber V(*parent);

            if (!uSquared.trySqrt(U)) {
                continue;
            }

            if (!vSquared.trySqrt(V)) {
                continue;
            }

            // U、V 当前都是非负根。
            // 根据 B 的符号调整 V，使 2UV = B。
            std::vector<Rational> vCoefficients = V.getCoefficients();

            if (B.isNegative()) {
                for (auto& coefficient : vCoefficients) {
                    coefficient = -coefficient;
                }
            }

            QuadraticNumber signedV(*parent, vCoefficients);
            QuadraticNumber candidate = compose(U, signedV);

            // 统一选择非负平方根
            if (candidate.isNegative()) {
                std::vector<Rational> coefficients =
                    candidate.getCoefficients();

                for (auto& coefficient : coefficients) {
                    coefficient = -coefficient;
                }

                // candidate = QuadraticNumber(field_, coefficients);
                candidate.coefficients_ = std::move(coefficients);
            }

            // 最终进行精确验证
            if (candidate * candidate == *this) {
                result.coefficients_ = candidate.coefficients_;
                return true;
            }
        }

        return false;
    }
}