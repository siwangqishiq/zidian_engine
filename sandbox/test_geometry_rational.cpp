
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "zidian.h"

using namespace zidian;

int passed = 0;
int failed = 0;

// ============================================================
// 测试辅助函数
// ============================================================

void check(bool condition, const std::string& name) {
    if (condition) {
        ++passed;
        std::cout << "[PASS] " << name << '\n';
    } else {
        ++failed;
        std::cout << "[FAIL] " << name << '\n';
    }
}

void checkEqual(
    const Rational& actual,
    const Rational& expected,
    const std::string& name
) {
    check(actual == expected, name);
}

template<typename Exception, typename Function>
void checkThrows(Function function, const std::string& name) {
    try {
        function();
        check(false, name);
    } catch (const Exception&) {
        check(true, name);
    } catch (...) {
        check(false, name + " (unexpected exception)");
    }
}

// ============================================================
// 1. 构造和约分
// ============================================================

void testConstruction() {
    std::cout << "\n=== Construction ===\n";

    checkEqual(
        Rational(),
        Rational(0),
        "default constructor"
    );

    checkEqual(
        Rational(2, 4),
        Rational(1, 2),
        "reduce fraction"
    );

    checkEqual(
        Rational(-2, 4),
        Rational(-1, 2),
        "reduce negative fraction"
    );

    checkEqual(
        Rational(2, -4),
        Rational(-1, 2),
        "negative denominator"
    );

    checkEqual(
        Rational(-2, -4),
        Rational(1, 2),
        "both negative"
    );

    checkEqual(
        Rational(0, 5),
        Rational(0, 1),
        "zero normalization"
    );

    check(
        Rational(1, 3).toString() == "1/3",
        "fraction toString"
    );

    check(
        Rational(5).toString() == "5",
        "integer toString"
    );

    checkThrows<std::invalid_argument>(
        [] {
            Rational(1, 0);
        },
        "zero denominator"
    );
}

// ============================================================
// 2. 四则运算
// ============================================================

void testArithmetic() {
    std::cout << "\n=== Arithmetic ===\n";

    Rational a(1, 2);
    Rational b(1, 3);

    checkEqual(
        a + b,
        Rational(5, 6),
        "addition"
    );

    checkEqual(
        a - b,
        Rational(1, 6),
        "subtraction"
    );

    checkEqual(
        a * b,
        Rational(1, 6),
        "multiplication"
    );

    checkEqual(
        a / b,
        Rational(3, 2),
        "division"
    );

    checkEqual(
        Rational(-1, 2) + Rational(1, 3),
        Rational(-1, 6),
        "addition with negative"
    );

    checkEqual(
        Rational(-1, 2) * Rational(-2, 3),
        Rational(1, 3),
        "multiplication with two negatives"
    );

    checkEqual(
        Rational(1, 2) / Rational(-1, 4),
        Rational(-2),
        "division with negative"
    );

    checkThrows<std::domain_error>(
        [] {
            Rational a(1, 2);
            Rational b(0);

            auto result = a / b;
            (void)result;
        },
        "division by zero"
    );
}

// ============================================================
// 3. 复合赋值
// ============================================================

void testCompoundAssignment() {
    std::cout << "\n=== Compound Assignment ===\n";

    Rational a(1, 2);

    a += Rational(1, 3);
    checkEqual(a, Rational(5, 6), "operator +=");

    a -= Rational(1, 3);
    checkEqual(a, Rational(1, 2), "operator -=");

    a *= Rational(2, 3);
    checkEqual(a, Rational(1, 3), "operator *=");

    a /= Rational(2, 3);
    checkEqual(a, Rational(1, 2), "operator /=");

    // 自我加法
    a += a;
    checkEqual(a, Rational(1), "self addition");

    // 自我乘法
    a = Rational(2, 3);
    a *= a;
    checkEqual(a, Rational(4, 9), "self multiplication");

    // 自我除法
    a = Rational(2, 3);
    a /= a;
    checkEqual(a, Rational(1), "self division");
}

// ============================================================
// 4. 比较运算
// ============================================================

void testComparison() {
    std::cout << "\n=== Comparison ===\n";

    Rational a(1, 2);
    Rational b(2, 4);
    Rational c(2, 3);

    check(a == b, "equal fractions");
    check(!(a != b), "not equal operator");

    check(a < c, "less than");
    check(a <= b, "less than or equal");
    check(c > a, "greater than");
    check(c >= a, "greater than or equal");

    check(
        Rational(-1, 2) < Rational(1, 3),
        "negative less than positive"
    );

    check(
        Rational(-2, 3) < Rational(-1, 3),
        "negative comparison"
    );

    check(
        Rational(-1, 2) == Rational(-2, 4),
        "negative equivalent fractions"
    );

    check(
        Rational(0, 5) == Rational(0, 100),
        "zero fraction comparison"
    );
}

// ============================================================
// 5. 符号判断
// ============================================================

void testSign() {
    std::cout << "\n=== Sign ===\n";

    Rational positive(1, 3);
    Rational negative(-1, 3);
    Rational zero(0);

    check(positive.isPositive(), "positive");
    check(negative.isNegative(), "negative");
    check(zero.isZero(), "zero");

    check(positive.sign() == 1, "positive sign");
    check(negative.sign() == -1, "negative sign");
    check(zero.sign() == 0, "zero sign");
}

// ============================================================
// 6. 大整数运算
// ============================================================

void testBigIntegerOperations() {
    std::cout << "\n=== Big Integer Operations ===\n";

    BigInt a("9223372036854775807");
    BigInt b("9223372036854775808");

    check(
        b > a,
        "BigInt greater than LLONG_MAX"
    );

    check(
        BigInt("-9223372036854775809") < a,
        "BigInt less than LLONG_MIN"
    );

    Rational r1(a);
    Rational r2(2);

    checkEqual(
        r1 * r2,
        Rational(BigInt("18446744073709551614")),
        "multiplication beyond LLONG_MAX"
    );

    checkEqual(
        r1 + r2,
        Rational(BigInt("9223372036854775809")),
        "addition beyond LLONG_MAX"
    );

    checkEqual(
        r1 - Rational(BigInt("9223372036854775809")),
        Rational(-2),
        "subtraction beyond LLONG_MAX"
    );

    checkEqual(
        Rational(BigInt("18446744073709551614")) / r2,
        r1,
        "division beyond LLONG_MAX"
    );
}

// ============================================================
// 7. 大整数构造和约分
// ============================================================

void testLargeFraction() {
    std::cout << "\n=== Large Fraction ===\n";

    BigInt a("123456789012345678901234567890");
    BigInt b("246913578024691357802469135780");

    Rational r(a, b);

    checkEqual(
        r,
        Rational(1, 2),
        "large fraction reduction"
    );

    check(
        r.toString() == "1/2",
        "large fraction toString"
    );

    Rational zero(
        BigInt(0),
        BigInt("-999999999999999999999999999999")
    );

    check(
        zero.isZero() &&
        zero.denominator() == BigInt(1),
        "large denominator zero normalization"
    );

    checkThrows<std::invalid_argument>(
        [] {
            Rational r(
                BigInt("12345678901234567890"),
                BigInt(0)
            );
        },
        "large numerator with zero denominator"
    );
}

// ============================================================
// 8. 大整数比较
// ============================================================

void testLargeComparison() {
    std::cout << "\n=== Large Comparison ===\n";

    BigInt a("999999999999999999999999999999");
    BigInt b("999999999999999999999999999998");

    Rational r1(a, b);
    Rational r2(b, a);

    check(
        r1 > r2,
        "compare large fractions"
    );

    check(
        r2 < r1,
        "reverse large fraction comparison"
    );

    check(
        Rational(a, a - BigInt(1)) > Rational(1),
        "large fraction greater than one"
    );

    check(
        Rational(b, a) < Rational(1),
        "large fraction less than one"
    );
}

// ============================================================
// 9. 大整数交叉约分
// ============================================================

void testLargeCrossReduction() {
    std::cout << "\n=== Large Cross Reduction ===\n";

    BigInt n("123456789012345678901234567890");

    Rational a(n, n - BigInt(1));
    Rational b(n - BigInt(1), n);

    checkEqual(
        a * b,
        Rational(1),
        "large fraction cross reduction"
    );

    checkEqual(
        a / a,
        Rational(1),
        "large fraction self division"
    );

    Rational c(n, n - BigInt(1));
    Rational d(
        -(n - BigInt(1)),
        n
    );

    checkEqual(
        c * d,
        Rational(-1),
        "negative large fraction cross reduction"
    );
}

// ============================================================
// 10. 大整数负数运算
// ============================================================

void testLargeNegative() {
    std::cout << "\n=== Large Negative ===\n";

    BigInt positive("123456789012345678901234567890");
    BigInt negative("-123456789012345678901234567890");

    Rational a(positive);
    Rational b(negative);

    checkEqual(
        a + b,
        Rational(0),
        "large positive plus negative"
    );

    checkEqual(
        a + b - Rational(1),
        Rational(-1),
        "large cancellation"
    );

    checkEqual(
        b * Rational(-1),
        a,
        "large negative multiplication"
    );

    checkEqual(
        b / Rational(-1),
        a,
        "large negative division"
    );

    check(
        b.isNegative(),
        "large negative sign"
    );
}

// ============================================================
// 11. 随机运算性质测试
// ============================================================

void testRandomOperations() {
    std::cout << "\n=== Random Operations ===\n";

    std::mt19937_64 rng(123456);

    std::uniform_int_distribution<long long> numeratorDist(
        -1000,
        1000
    );

    std::uniform_int_distribution<long long> denominatorDist(
        1,
        1000
    );

    bool additionOk = true;
    bool subtractionOk = true;
    bool multiplicationOk = true;
    bool divisionOk = true;
    bool commutativeOk = true;
    bool comparisonOk = true;

    constexpr int TEST_COUNT = 1000;

    for (int i = 0; i < TEST_COUNT; ++i) {
        Rational a(
            numeratorDist(rng),
            denominatorDist(rng)
        );

        Rational b(
            numeratorDist(rng),
            denominatorDist(rng)
        );

        // (a + b) - b = a
        if ((a + b) - b != a) {
            additionOk = false;
        }

        // (a - b) + b = a
        if ((a - b) + b != a) {
            subtractionOk = false;
        }

        // 乘法交换律
        if (a * b != b * a) {
            multiplicationOk = false;
        }

        // 除法逆运算
        if (!b.isZero()) {
            if ((a * b) / b != a) {
                divisionOk = false;
            }
        }

        // 加法、乘法交换律
        if (a + b != b + a || a * b != b * a) {
            commutativeOk = false;
        }

        // 比较一致性
        if ((a < b) != (b > a)) {
            comparisonOk = false;
        }

        if ((a <= b) != (a < b || a == b)) {
            comparisonOk = false;
        }
    }

    check(additionOk, "random addition property");
    check(subtractionOk, "random subtraction property");
    check(multiplicationOk, "random multiplication property");
    check(divisionOk, "random division property");
    check(commutativeOk, "random commutative property");
    check(comparisonOk, "random comparison consistency");
}

// ============================================================
// 测试入口
// ============================================================

void TestGeometryRational() {
    passed = 0;
    failed = 0;

    testConstruction();
    testArithmetic();
    testCompoundAssignment();
    testComparison();
    testSign();

    testBigIntegerOperations();
    testLargeFraction();
    testLargeComparison();
    testLargeCrossReduction();
    testLargeNegative();

    testRandomOperations();

    std::cout << "\n============================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "============================\n";
}
