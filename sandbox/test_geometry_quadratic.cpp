#include <iostream>
#include <stdexcept>
#include <string>
#include "zidian.h"

using namespace zidian;
using namespace std;

static int passed = 0;
static int failed = 0;

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

// --------------------------------------------------
// 测试数域
// --------------------------------------------------

int TestGeometryQuadratic() {

    // 基础数域 Q
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
    // 1. 基础数域 Q
    // --------------------------------------------------

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

        expect(a + b == QuadraticNumber(Q, Rational(5, 6)),
               "Addition failed");

        expect(a - b == QuadraticNumber(Q, Rational(1, 6)),
               "Subtraction failed");

        expect(a * b == QuadraticNumber(Q, Rational(1, 6)),
               "Multiplication failed");
        
        expect(a / b == QuadraticNumber(Q, Rational(3, 2)),
               "Division failed");
    });

    // --------------------------------------------------
    // 2. 第一层扩张 Q(sqrt(2))
    // --------------------------------------------------

    test("Level 1 - Construction", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(2));

        // 1 + 2*sqrt(2)
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

        // (1 + 2*sqrt(2))^2 = 9 + 4*sqrt(2)
        expectEqual(
            a * a,
            { Rational(9), Rational(4) },
            "Multiplication failed"
        );
    });

    test("Level 1 - Division", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(1));
        QuadraticNumber b(Qsqrt2, Rational(1), Rational(-1));

        // (1+sqrt(2))/(1-sqrt(2)) = -3-2*sqrt(2)
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

        // 3^2 - 2^2*2 = 1
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

    // --------------------------------------------------
    // 3. 第二层扩张 Q(sqrt(2), sqrt(3))
    //
    // 系数顺序：
    // [1, sqrt(2), sqrt(3), sqrt(6)]
    // --------------------------------------------------

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
        // (1+sqrt(2)) * (1+sqrt(3))
        QuadraticNumber a(
            Qsqrt2sqrt3,
            vector<Rational>{
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1)
            }
        );

        // 1 + sqrt(2) + sqrt(3) + sqrt(6)
        expectEqual(
            a,
            {
                Rational(1),
                Rational(1),
                Rational(1),
                Rational(1)
            },
            "Unexpected coefficients"
        );

        // 再平方：
        // (1+sqrt(2))^2 * (1+sqrt(3))^2
        // = (3+2sqrt(2)) * (4+2sqrt(3))
        // = 12 + 8sqrt(2) + 6sqrt(3) + 4sqrt(6)
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
        // 2 - sqrt(3) > 0
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

        // sqrt(3) - 2 < 0
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

        // 对最外层 sqrt(3) 取共轭
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

        // A = 1+sqrt(2), B = 1+sqrt(2)
        // norm = A^2 - 3*B^2
        //      = -6 - 4*sqrt(2)
        expectEqual(
            a.norm(),
            {
                Rational(-6),
                Rational(-4)
            },
            "Level 2 norm failed"
        );
    });

    // --------------------------------------------------
    // 4. 第三层扩张
    //
    // 系数顺序：
    // [1, sqrt(2), sqrt(3), sqrt(6),
    //  sqrt(5), sqrt(10), sqrt(15), sqrt(30)]
    // --------------------------------------------------

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
        // (1+sqrt(2)+sqrt(3)+sqrt(6)) * (1+sqrt(5))
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

        // 这里验证：
        // (1+sqrt(2)+sqrt(3)+sqrt(6))^2
        // 乘以 (1+sqrt(5))^2
        //
        // 先计算前者平方：
        // 12 + 8sqrt(2) + 6sqrt(3) + 4sqrt(6)
        //
        // 后者平方：
        // 6 + 2sqrt(5)
        //
        // 最终系数：
        // [72, 48, 36, 24, 24, 16, 12, 8]

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

    // --------------------------------------------------
    // 5. 精确零判断
    // --------------------------------------------------

    test("Exact Zero - Algebraic Cancellation", [&]() {
        QuadraticNumber a(Qsqrt2, Rational(1), Rational(1));

        QuadraticNumber result =
            a * a -
            QuadraticNumber(Qsqrt2, Rational(3), Rational(2));

        expect(result.isZero(), "Result should be exactly zero");
        expect(result.sign() == 0, "Zero sign should be 0");
    });

    // --------------------------------------------------
    // 6. 比较运算
    // --------------------------------------------------

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

    // --------------------------------------------------
    // 7. 异常处理
    // --------------------------------------------------

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

    // --------------------------------------------------
    // 测试结果
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

