#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include "zidian.h"

using namespace zidian;

int passed = 0;
int failed = 0;

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

// 1. 构造和约分
void testConstruction() {
    std::cout << "\n=== Construction ===\n";

    checkEqual(Rational(), Rational(0), "default constructor");

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
        [] { Rational(1, 0); },
        "zero denominator"
    );
}

// 2. 四则运算
void testArithmetic() {
    std::cout << "\n=== Arithmetic ===\n";

    Rational a(1, 2);
    Rational b(1, 3);

    checkEqual(a + b, Rational(5, 6), "addition");
    checkEqual(a - b, Rational(1, 6), "subtraction");
    checkEqual(a * b, Rational(1, 6), "multiplication");
    checkEqual(a / b, Rational(3, 2), "division");

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

// 3. 复合赋值
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

    // 自我赋值
    a += a;
    checkEqual(a, Rational(1), "self addition");

    a = Rational(2, 3);
    a *= a;
    checkEqual(a, Rational(4, 9), "self multiplication");

    a = Rational(2, 3);
    a /= a;
    checkEqual(a, Rational(1), "self division");
}

// 4. 比较运算
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

    check(Rational(-1, 2) < Rational(1, 3),
          "negative less than positive");

    check(Rational(-2, 3) < Rational(-1, 3),
          "negative comparison");
}

// 5. 符号判断
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

// 6. 溢出检测
void testOverflow() {
    std::cout << "\n=== Overflow ===\n";

    using Integer = Integer;

    const Integer max = std::numeric_limits<Integer>::max();

    checkThrows<std::overflow_error>(
        [max] {
            Rational a(max);
            Rational b(2);
            auto result = a * b;
            (void)result;
        },
        "multiplication overflow"
    );

    checkThrows<std::overflow_error>(
        [max] {
            Rational a(max);
            Rational b(1);
            a += b;
        },
        "addition overflow"
    );
}

// 7. LLONG_MIN 边界测试
void testMinInteger() {
    std::cout << "\n=== LLONG_MIN Boundary ===\n";

    using Integer = Integer;

    const Integer min = std::numeric_limits<Integer>::min();

    // 单参数构造
    Rational a(min);

    check(
        a.numerator() == min && a.denominator() == 1,
        "construct LLONG_MIN"
    );

    // 双参数构造
    // 这个测试预计会暴露当前 gcd() 的问题
    try {
        Rational b(min, 1);

        check(
            b.numerator() == min && b.denominator() == 1,
            "construct LLONG_MIN / 1"
        );
    } catch (...) {
        check(false, "construct LLONG_MIN / 1");
    }

    // 零的分母虽然是最小负数，但结果仍然应该是零
    try {
        Rational c(0, min);

        check(
            c.isZero() && c.denominator() == 1,
            "zero with LLONG_MIN denominator"
        );
    } catch (...) {
        check(false, "zero with LLONG_MIN denominator");
    }

    // -LLONG_MIN 超出 long long 范围，应该抛出异常
    checkThrows<std::overflow_error>(
        [min] {
            Rational a(min, -1);
            (void)a;
        },
        "unrepresentable positive numerator"
    );
}


// 8. 比较运算溢出测试
void testComparisonOverflow() {
    std::cout << "\n=== Comparison Overflow ===\n";

    using Integer = Integer;

    const Integer max = std::numeric_limits<Integer>::max();

    Rational a(max, max - 1);
    Rational b(max - 1, max);

    bool result = false;
    bool exceptionThrown = false;

    try {
        result = a > b;
    } catch (...) {
        exceptionThrown = true;
    }

    check(
        !exceptionThrown && result,
        "compare large fractions without overflow"
    );
}

// 9. 大数交叉约分测试
void testCrossReduction() {
    std::cout << "\n=== Cross Reduction ===\n";

    using Integer = Integer;

    const Integer max = std::numeric_limits<Integer>::max();

    Rational a(max, max - 1);
    Rational b(max - 1, max);

    try {
        Rational result = a * b;

        checkEqual(
            result,
            Rational(1),
            "large fraction cross reduction"
        );
    } catch (...) {
        check(false, "large fraction cross reduction");
    }

    // 再测试不同符号
    try {
        Rational c(max, max - 1);
        Rational d(-(max - 1), max);

        checkEqual(
            c * d,
            Rational(-1),
            "negative large fraction cross reduction"
        );
    } catch (...) {
        check(false, "negative large fraction cross reduction");
    }
}

// 10. 随机运算性质测试
void testRandomOperations() {
    std::cout << "\n=== Random Operations ===\n";

    std::mt19937_64 rng(123456);

    std::uniform_int_distribution<long long> numeratorDist(-1000, 1000);
    std::uniform_int_distribution<long long> denominatorDist(1, 1000);

    bool additionOk = true;
    bool subtractionOk = true;
    bool multiplicationOk = true;
    bool divisionOk = true;
    bool commutativeOk = true;

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
    }

    check(additionOk, "random addition property");
    check(subtractionOk, "random subtraction property");
    check(multiplicationOk, "random multiplication property");
    check(divisionOk, "random division property");
    check(commutativeOk, "random commutative property");
}

void TestGeometryRational() {
    testConstruction();
    testArithmetic();
    testCompoundAssignment();
    testComparison();
    testSign();
    testOverflow();

    testMinInteger();
    testComparisonOverflow();
    testCrossReduction();
    testRandomOperations();


    std::cout << "\n============================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "============================\n";
}
