
#include <iostream>
#include <stdexcept>
#include <string>
#include <functional>
#include <vector>
#include <random>

#include "zidian.h"

using namespace zidian;
using namespace std;

static int passed = 0;
static int failed = 0;

// ==================================================
// 测试框架
// ==================================================

static void test(const string& name, const function<void()>& fn) {
    try {
        fn();
        cout << "[PASS] " << name << endl;
        ++passed;
    }
    catch (const exception& e) {
        cout << "[FAIL] " << name << ": " << e.what() << endl;
        ++failed;
    }
}

static void expect(bool condition, const string& message) {
    if (!condition) {
        throw runtime_error(message);
    }
}

static void expectEqual(
    const QuadraticNumber& actual,
    const vector<Rational>& expected,
    const string& message
) {
    expect(
        actual.getCoefficients() == expected,
        message
    );
}

static void expectRational(
    const Rational& actual,
    const Rational& expected,
    const string& message
) {
    expect(actual == expected, message);
}

static void expectThrow(
    const function<void()>& fn,
    const string& message
) {
    bool thrown = false;

    try {
        fn();
    }
    catch (const exception&) {
        thrown = true;
    }

    expect(thrown, message);
}

// ==================================================
// 1. 基础数域 Q
// ==================================================

static void testBaseField(QuadraticField& Q) {

    test("Base Field - Construction", [&]() {
        QuadraticNumber a(Q, Rational(3, 2));

        expectEqual(
            a,
            { Rational(3, 2) },
            "Q should contain one coefficient"
        );
    });

    test("Base Field - Arithmetic", [&]() {
        QuadraticNumber a(Q, Rational(1, 2));
        QuadraticNumber b(Q, Rational(1, 3));

        expect(
            a + b == QuadraticNumber(Q, Rational(5, 6)),
            "Addition failed"
        );

        expect(
            a - b == QuadraticNumber(Q, Rational(1, 6)),
            "Subtraction failed"
        );

        expect(
            a * b == QuadraticNumber(Q, Rational(1, 6)),
            "Multiplication failed"
        );

        expect(
            a / b == QuadraticNumber(Q, Rational(3, 2)),
            "Division failed"
        );
    });
}

// ==================================================
// 2. 第一层扩张 Q(sqrt(2))
// ==================================================

static void testLevel1(QuadraticField& Qsqrt2) {

    test("Level 1 - Construction", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(2));

        expectEqual(
            a,
            { Rational(1), Rational(2) },
            "Unexpected coefficients"
        );
    });

    test("Level 1 - Addition", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(2));
        QuadraticNumber b(Qsqrt2, Rational(3), Rational(4));

        expectEqual(
            a + b,
            { Rational(4), Rational(6) },
            "Addition failed"
        );
    });

    test("Level 1 - Multiplication", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(2));

        expectEqual(
            a * a,
            { Rational(9), Rational(4) },
            "Multiplication failed"
        );
    });

    test("Level 1 - Division", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(1));
        QuadraticNumber b(Qsqrt2, Rational(1), Rational(-1));

        expectEqual(
            a / b,
            { Rational(-3), Rational(-2) },
            "Division failed"
        );
    });

    test("Level 1 - Conjugate", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(3), Rational(2));

        expectEqual(
            a.conjugate(),
            { Rational(3), Rational(-2) },
            "Conjugate failed"
        );
    });

    test("Level 1 - Norm", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(3), Rational(2));

        expectEqual(
            a.norm(),
            { Rational(1) },
            "Norm failed"
        );
    });

    test("Level 1 - Sign", [&]() {
        QuadraticNumber positive(Qsqrt2, Rational(0), Rational(1));
        QuadraticNumber negative(Qsqrt2, Rational(1), Rational(-1));
        QuadraticNumber zero(Qsqrt2, Rational(0), Rational(0));

        expect(positive.sign() > 0, "sqrt(2) should be positive");
        expect(negative.sign() < 0, "1-sqrt(2) should be negative");
        expect(zero.sign() == 0, "Zero sign should be 0");
    });
}

// ==================================================
// 3. 第二层扩张 Q(sqrt(2), sqrt(3))
// ==================================================

static void testLevel2(QuadraticField& Qsqrt2sqrt3) {

    test("Level 2 - Construction", [&]() {
        QuadraticNumber a(
            Qsqrt2sqrt3,
            vector<Rational>{
                Rational(1),
                Rational(2),
                Rational(3),
                Rational(4)
            }
        );

        expectEqual(
            a,
            {
                Rational(1),
                Rational(2),
                Rational(3),
                Rational(4)
            },
            "Unexpected coefficients"
        );
    });

    test("Level 2 - Multiplication", [&]() {
        QuadraticNumber a(
            Qsqrt2sqrt3,
            vector<Rational>{
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1)
            }
        );

        expectEqual(
            a * a,
            {
                Rational(12),
                Rational(8),
                Rational(6),
                Rational(4)
            },
            "Level 2 multiplication failed"
        );
    });

    test("Level 2 - Exact Sign", [&]() {
        QuadraticNumber a(
            Qsqrt2sqrt3,
            vector<Rational>{
                Rational(2),
                Rational(0),
                Rational(-1),
                Rational(0)
            }
        );

        expect(a.sign() > 0, "2-sqrt(3) should be positive");
        expect((-a).sign() < 0, "sqrt(3)-2 should be negative");
    });

    test("Level 2 - Conjugate", [&]() {
        QuadraticNumber a(
            Qsqrt2sqrt3,
            vector<Rational>{
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1)
            }
        );

        expectEqual(
            a.conjugate(),
            {
                Rational(1),
                Rational(1),
                Rational(-1),
                Rational(-1)
            },
            "Level 2 conjugate failed"
        );
    });

    test("Level 2 - Norm", [&]() {
        QuadraticNumber a(
            Qsqrt2sqrt3,
            vector<Rational>{
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1)
            }
        );

        expectEqual(
            a.norm(),
            {
                Rational(-6),
                Rational(-4)
            },
            "Level 2 norm failed"
        );
    });
}

// ==================================================
// 4. 第三层扩张
// Q(sqrt(2), sqrt(3), sqrt(5))
// ==================================================

static void testLevel3(QuadraticField& Qsqrt2sqrt3sqrt5) {

    test("Level 3 - Construction", [&]() {
        expect(
            Qsqrt2sqrt3sqrt5.getLevel() == 3,
            "Level should be 3"
        );

        expect(
            Qsqrt2sqrt3sqrt5.getDimension() == 8,
            "Dimension should be 8"
        );
    });

    test("Level 3 - Multiplication", [&]() {
        QuadraticNumber a(
            Qsqrt2sqrt3sqrt5,
            vector<Rational>{
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1)
            }
        );

        expectEqual(
            a * a,
            {
                Rational(72),
                Rational(48),
                Rational(36),
                Rational(24),
                Rational(24),
                Rational(16),
                Rational(12),
                Rational(8)
            },
            "Level 3 multiplication failed"
        );
    });
}

// ==================================================
// 5. 精确零判断
// ==================================================

static void testExactZero(QuadraticField& Qsqrt2) {

    test("Exact Zero - Algebraic Cancellation", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(1));

        QuadraticNumber result =
            a * a -
            QuadraticNumber(Qsqrt2, Rational(3), Rational(2));

        expect(result.isZero(), "Result should be exactly zero");
        expect(result.sign() == 0, "Zero sign should be 0");
    });
}

// ==================================================
// 6. 比较运算
// ==================================================

static void testComparison(QuadraticField& Qsqrt2) {

    test("Comparison - Less And Greater", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(0));
        QuadraticNumber b(Qsqrt2, Rational(0), Rational(1));

        expect(a < b, "1 should be less than sqrt(2)");
        expect(b > a, "sqrt(2) should be greater than 1");
        expect(a <= b, "<= failed");
        expect(b >= a, ">= failed");
    });

    test("Comparison - Equality", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(2));
        QuadraticNumber b(Qsqrt2, Rational(1), Rational(2));
        QuadraticNumber c(Qsqrt2, Rational(2), Rational(1));

        expect(a == b, "Equal values should compare equal");
        expect(a != c, "Different values should not compare equal");
    });
}

// ==================================================
// 7. 异常处理
// ==================================================

static void testExceptions(QuadraticField& Q, QuadraticField& Qsqrt2) {

    test("Exception - Division By Zero", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(1));
        QuadraticNumber zero(Qsqrt2);

        expectThrow(
            [&]() {
                auto result = a / zero;
                (void)result;
            },
            "Division by zero should throw"
        );
    });

    test("Exception - Different Fields", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(1));
        QuadraticNumber b(Q, Rational(1));

        expectThrow(
            [&]() {
                auto result = a + b;
                (void)result;
            },
            "Different fields should not allow addition"
        );
    });
}

// ==================================================
// 8. 平方根判定：基础域 Q
// ==================================================

static void testSqrtBaseField() {

    QuadraticField Q;
    QuadraticNumber result(Q);

    test("Sqrt Base - Perfect Square", [&]() {
        QuadraticNumber x(Q, Rational(4));

        expect(x.trySqrt(result), "sqrt(4) should succeed");

        expectEqual(
            result,
            { Rational(2) },
            "sqrt(4) should equal 2"
        );
    });

    test("Sqrt Base - Non Perfect Square", [&]() {
        QuadraticNumber x(Q, Rational(2));

        expect(!x.trySqrt(result), "sqrt(2) should not belong to Q");
    });

    test("Sqrt Base - Rational Fraction", [&]() {
        QuadraticNumber x(Q, Rational(25, 16));

        expect(x.trySqrt(result), "sqrt(25/16) should succeed");

        expectEqual(
            result,
            { Rational(5, 4) },
            "sqrt(25/16) should equal 5/4"
        );
    });

    test("Sqrt Base - Negative Number", [&]() {
        QuadraticNumber x(Q, Rational(-4));

        expect(!x.trySqrt(result), "Negative number should not have real square root");
    });

    test("Sqrt Base - Zero", [&]() {
        QuadraticNumber x(Q, Rational(0));

        expect(x.trySqrt(result), "sqrt(0) should succeed");
        expect(result.isZero(), "sqrt(0) should equal zero");
    });
}

// ==================================================
// 9. 平方根判定：第一层扩张
// ==================================================

static void testSqrtSingleExtension() {

    QuadraticField Q;

    QuadraticNumber radicand(Q, Rational(2));
    QuadraticField Q2(Q, radicand);

    QuadraticNumber result(Q2);

    test("Sqrt Level 1 - Rational Square Root", [&]() {
        // sqrt(8) = 2*sqrt(2)
        QuadraticNumber x(Q2, Rational(8));

        expect(x.trySqrt(result), "sqrt(8) should succeed");

        expectEqual(
            result,
            { Rational(0), Rational(2) },
            "sqrt(8) should equal 2*sqrt(2)"
        );
    });

    test("Sqrt Level 1 - Not In Current Field", [&]() {
        QuadraticNumber x(Q2, Rational(3));

        expect(!x.trySqrt(result), "sqrt(3) should not belong to Q(sqrt(2))");
    });

    test("Sqrt Level 1 - Positive Coefficient", [&]() {
        // sqrt(3 + 2*sqrt(2)) = 1 + sqrt(2)
        QuadraticNumber x(Q2, Rational(3), Rational(2));

        expect(x.trySqrt(result), "sqrt(3 + 2*sqrt(2)) should succeed");

        expectEqual(
            result,
            { Rational(1), Rational(1) },
            "Unexpected square root"
        );
    });

    test("Sqrt Level 1 - Negative Coefficient", [&]() {
        // sqrt(3 - 2*sqrt(2)) = sqrt(2) - 1
        QuadraticNumber x(Q2, Rational(3), Rational(-2));

        expect(x.trySqrt(result), "sqrt(3 - 2*sqrt(2)) should succeed");

        expectEqual(
            result,
            { Rational(-1), Rational(1) },
            "Unexpected square root"
        );

        expect(result.isPositive(), "Square root should be positive");
    });
}

// ==================================================
// 10. 平方根判定：第二层扩张
// ==================================================

static void testSqrtDoubleExtension() {

    QuadraticField Q;

    QuadraticNumber radicand2(Q, Rational(2));
    QuadraticField Q2(Q, radicand2);

    QuadraticNumber radicand3(Q2, Rational(3));
    QuadraticField Q23(Q2, radicand3);

    QuadraticNumber result(Q23);

    test("Sqrt Level 2 - sqrt(6)", [&]() {
        // sqrt(6) = sqrt(2)*sqrt(3)
        // 被开方数是 6，而不是 sqrt(6)
        QuadraticNumber x(Q23, Rational(6));

        expect(x.trySqrt(result), "sqrt(6) should succeed");

        expectEqual(
            result,
            {
                Rational(0),
                Rational(0),
                Rational(0),
                Rational(1)
            },
            "sqrt(6) should equal the sqrt(6) basis element"
        );
    });

    test("Sqrt Level 2 - Nested Square Root", [&]() {
        // sqrt(5 + 2*sqrt(6)) = sqrt(2) + sqrt(3)
        QuadraticNumber x(
            Q23,
            vector<Rational>{
                Rational(5),
                Rational(0),
                Rational(0),
                Rational(2)
            }
        );

        expect(x.trySqrt(result), "Nested square root should succeed");

        expectEqual(
            result,
            {
                Rational(0),
                Rational(1),
                Rational(1),
                Rational(0)
            },
            "Unexpected square root"
        );
    });

    test("Sqrt Level 2 - Rational Part", [&]() {
        // sqrt(7 + 4*sqrt(3)) = 2 + sqrt(3)
        QuadraticNumber x(
            Q23,
            vector<Rational>{
                Rational(7),
                Rational(0),
                Rational(4),
                Rational(0)
            }
        );

        expect(x.trySqrt(result), "sqrt(7 + 4*sqrt(3)) should succeed");

        expectEqual(
            result,
            {
                Rational(2),
                Rational(0),
                Rational(1),
                Rational(0)
            },
            "Unexpected square root"
        );
    });

    test("Sqrt Level 2 - Not In Current Field", [&]() {
        QuadraticNumber x(Q23, Rational(11));

        expect(!x.trySqrt(result), "sqrt(11) should not belong to current field");
    });
}


void testSqrtLevel3(QuadraticField& Q235) {
    test("Sqrt Level 3 - sqrt(30)", [&]() {
        QuadraticNumber x(Q235, Rational(30));
        QuadraticNumber result(Q235);

        expect(x.trySqrt(result), "sqrt(30) should exist");

        vector<Rational> expected = {
            Rational(0), Rational(0), Rational(0), Rational(0),
            Rational(0), Rational(0), Rational(0), Rational(1)
        };

        expectEqual(result, expected, "sqrt(30) should equal basis element");
    });

    test("Sqrt Level 3 - sqrt(11 + 2sqrt(30))", [&]() {
        QuadraticNumber x(Q235, vector<Rational>{
            Rational(11), Rational(0), Rational(0), Rational(2),
            Rational(0), Rational(0), Rational(0), Rational(0)
        });

        QuadraticNumber result(Q235);

        expect(x.trySqrt(result), "sqrt(11 + 2sqrt(30)) should exist");

        vector<Rational> expected = {
            Rational(0), Rational(0), Rational(0), Rational(1),
            Rational(0), Rational(0), Rational(1), Rational(0)
        };

        expectEqual(result, expected, "result should equal sqrt(5) + sqrt(6)");
    });

    test("Sqrt Level 3 - sqrt(9 + 4sqrt(5))", [&]() {
        QuadraticNumber x(Q235, vector<Rational>{
            Rational(9), Rational(0), Rational(0), Rational(0),
            Rational(4), Rational(0), Rational(0), Rational(0)
        });

        QuadraticNumber result(Q235);

        expect(x.trySqrt(result), "sqrt(9 + 4sqrt(5)) should exist");

        vector<Rational> expected = {
            Rational(2), Rational(0), Rational(0), Rational(0),
            Rational(1), Rational(0), Rational(0), Rational(0)
        };

        expectEqual(result, expected, "result should equal 2 + sqrt(5)");
    });

    test("Sqrt Level 3 - sqrt(17) does not exist", [&]() {
        QuadraticNumber x(Q235, Rational(17));
        QuadraticNumber result(Q235);

        expect(!x.trySqrt(result), "sqrt(17) should not exist in this field");
    });
}

void testSqrtRandomProperties(QuadraticField& field) {
    test("Sqrt Random Properties", [&]() {
        mt19937 rng(20261111);

        // uniform_int_distribution<int> numeratorDist(-2, 2);
        uniform_int_distribution<int> numeratorDist(-1, 1);
        uniform_int_distribution<int> denominatorDist(1, 3);

        const int testCount = 20;

        for (int i = 0; i < testCount; ++i) {
            // vector<Rational> coefficients;

            // for (size_t j = 0; j < field.getDimension(); ++j) {
            //     int numerator = numeratorDist(rng);
            //     int denominator = denominatorDist(rng);

            //     coefficients.emplace_back(numerator, denominator);
            // }
            vector<Rational> coefficients;

            for (size_t j = 0; j < field.getDimension(); ++j) {
                coefficients.emplace_back(numeratorDist(rng));
            }

            QuadraticNumber y(field, coefficients);

            // x = y²
            QuadraticNumber x = y * y;

            QuadraticNumber result(field);

            expect(x.trySqrt(result), "sqrt(y * y) should always exist");

            expect(result * result == x, "result squared should equal x");

            expect(!result.isNegative(), "result should be non-negative");
        }
    });
}


// ==================================================
// 测试入口
// ==================================================

int TestGeometryQuadratic() {

    passed = 0;
    failed = 0;

    // --------------------------------------------------
    // 建立数域扩张链
    // --------------------------------------------------

    QuadraticField Q;

    // 第一层：Q(sqrt(2))
    QuadraticNumber sqrt2Radicand(Q, Rational(2));
    QuadraticField Qsqrt2(Q, sqrt2Radicand);

    // 第二层：Q(sqrt(2), sqrt(3))
    QuadraticNumber sqrt3Radicand(Qsqrt2, Rational(3));
    QuadraticField Qsqrt2sqrt3(Qsqrt2, sqrt3Radicand);

    // 第三层：Q(sqrt(2), sqrt(3), sqrt(5))
    QuadraticNumber sqrt5Radicand(Qsqrt2sqrt3, Rational(5));
    QuadraticField Qsqrt2sqrt3sqrt5(
        Qsqrt2sqrt3,
        sqrt5Radicand
    );

    // --------------------------------------------------
    // 执行测试
    // --------------------------------------------------

    testBaseField(Q);
    testLevel1(Qsqrt2);
    testLevel2(Qsqrt2sqrt3);
    testLevel3(Qsqrt2sqrt3sqrt5);

    testExactZero(Qsqrt2);
    testComparison(Qsqrt2);
    testExceptions(Q, Qsqrt2);

    testSqrtBaseField();
    testSqrtSingleExtension();
    testSqrtDoubleExtension();

    testSqrtRandomProperties(Q);
    testSqrtRandomProperties(Qsqrt2);
    testSqrtRandomProperties(Qsqrt2sqrt3);
    testSqrtRandomProperties(Qsqrt2sqrt3sqrt5);

    // --------------------------------------------------
    // 输出测试结果
    // --------------------------------------------------

    cout << endl;
    cout << "================================" << endl;
    cout << "QuadraticNumber Test Results" << endl;
    cout << "================================" << endl;

    cout << "Passed: " << passed << endl;
    cout << "Failed: " << failed << endl;
    cout << "Total : " << passed + failed << endl;

    return failed == 0 ? 0 : 1;
}
