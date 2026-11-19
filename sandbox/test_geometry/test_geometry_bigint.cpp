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

static void test(const string& name, const function<void()>& fn) {
    try {
        fn();
        cout << "[PASS] " << name << endl;
        ++passed;
    } catch (const exception& e) {
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
    const BigInt& actual,
    const BigInt& expected,
    const string& message
) {
    expect(actual == expected,
        message + ", actual=" + actual.toString()
        + ", expected=" + expected.toString());
}

static void expectString(
    const BigInt& actual,
    const string& expected,
    const string& message
) {
    expect(actual.toString() == expected,
        message + ", actual=" + actual.toString()
        + ", expected=" + expected);
}

void testBigIntConstruction() {
    test("BigInt - Construction", []() {
        expectString(BigInt(), "0", "default construction");

        expectString(BigInt(123), "123", "positive integer");

        expectString(BigInt(-123), "-123", "negative integer");

        expectString(BigInt("12345678901234567890"),
            "12345678901234567890", "large integer");

        expectString(BigInt("-12345678901234567890"),
            "-12345678901234567890", "large negative integer");

        expectString(BigInt("00000123"), "123", "leading zeros");

        expectString(BigInt("-000000"), "0", "negative zero");

        expectString(
            BigInt(numeric_limits<long long>::min()),
            "-9223372036854775808",
            "LLONG_MIN construction"
        );
    });

    test("BigInt - Invalid Construction", []() {
        bool thrown = false;

        try {
            BigInt value("12a34");
        } catch (const invalid_argument&) {
            thrown = true;
        }

        expect(thrown, "invalid character should throw");

        thrown = false;

        try {
            BigInt value("");
        } catch (const invalid_argument&) {
            thrown = true;
        }

        expect(thrown, "empty string should throw");
    });
}

void testBigIntAddition() {
    test("BigInt - Addition", []() {
        expectEqual(BigInt(123) + BigInt(456),
            BigInt(579), "positive addition");

        expectEqual(BigInt(-123) + BigInt(-456),
            BigInt(-579), "negative addition");

        expectEqual(BigInt(123) + BigInt(-456),
            BigInt(-333), "mixed addition");

        expectEqual(BigInt(456) + BigInt(-123),
            BigInt(333), "mixed addition 2");

        expectEqual(
            BigInt("999999999999999999999999")
            + BigInt(1),
            BigInt("1000000000000000000000000"),
            "carry propagation"
        );

        expectEqual(BigInt(123) + BigInt(-123),
            BigInt(0), "zero result");
    });
}

void testBigIntSubtraction() {
    test("BigInt - Subtraction", []() {
        expectEqual(BigInt(456) - BigInt(123),
            BigInt(333), "positive subtraction");

        expectEqual(BigInt(123) - BigInt(456),
            BigInt(-333), "negative result");

        expectEqual(BigInt(-123) - BigInt(-456),
            BigInt(333), "negative subtraction");

        expectEqual(BigInt(123) - BigInt(-456),
            BigInt(579), "mixed subtraction");

        expectEqual(
            BigInt("1000000000000000000000000")
            - BigInt(1),
            BigInt("999999999999999999999999"),
            "borrow propagation"
        );
    });
}

void testBigIntMultiplication() {
    test("BigInt - Multiplication", []() {
        expectEqual(BigInt(12) * BigInt(13),
            BigInt(156), "small multiplication");

        expectEqual(BigInt(-12) * BigInt(13),
            BigInt(-156), "negative multiplication");

        expectEqual(BigInt(-12) * BigInt(-13),
            BigInt(156), "negative times negative");

        expectEqual(BigInt(123456789) * BigInt(0),
            BigInt(0), "multiply by zero");

        expectEqual(
            BigInt("12345678901234567890")
            * BigInt("98765432109876543210"),
            BigInt("1219326311370217952237463801111263526900"),
            "large multiplication"
        );
    });
}

void testBigIntDivision() {
    test("BigInt - Division And Modulo", []() {
        expectEqual(BigInt(10) / BigInt(3),
            BigInt(3), "integer division");

        expectEqual(BigInt(10) % BigInt(3),
            BigInt(1), "integer modulo");

        expectEqual(BigInt(-10) / BigInt(3),
            BigInt(-3), "negative division");

        expectEqual(BigInt(-10) % BigInt(3),
            BigInt(-1), "negative modulo");

        expectEqual(BigInt(10) / BigInt(-3),
            BigInt(-3), "negative divisor");

        expectEqual(BigInt(10) % BigInt(-3),
            BigInt(1), "negative divisor modulo");

        expectEqual(BigInt(123) / BigInt(456),
            BigInt(0), "dividend smaller than divisor");

        expectEqual(BigInt(123) % BigInt(456),
            BigInt(123), "remainder when dividend smaller");
    });

    test("BigInt - Large Division", []() {
        BigInt a("123456789012345678901234567890");
        BigInt b("9876543210");

        BigInt q = a / b;
        BigInt r = a % b;

        expectEqual(q * b + r, a, "division identity");
        expect(r >= BigInt(0), "remainder should be non-negative");
        expect(r < b, "remainder should be smaller than divisor");
    });

    test("BigInt - Division By Zero", []() {
        bool thrown = false;

        try {
            BigInt value = BigInt(123) / BigInt(0);
        } catch (const domain_error&) {
            thrown = true;
        }

        expect(thrown, "division by zero should throw");
    });
}

void testBigIntComparison() {
    test("BigInt - Comparison", []() {
        expect(BigInt(123) < BigInt(456), "less than");
        expect(BigInt(456) > BigInt(123), "greater than");
        expect(BigInt(123) == BigInt(123), "equality");
        expect(BigInt(123) != BigInt(456), "inequality");

        expect(BigInt(-456) < BigInt(-123), "negative comparison");
        expect(BigInt(-123) < BigInt(0), "negative less than zero");
        expect(BigInt(0) < BigInt(123), "zero less than positive");

        expect(BigInt("999999999999999999999")
            < BigInt("1000000000000000000000"),
            "large comparison");
    });
}

void testBigIntGcd() {
    test("BigInt - GCD", []() {
        expectEqual(BigInt::gcd(BigInt(12), BigInt(18)),
            BigInt(6), "gcd(12, 18)");

        expectEqual(BigInt::gcd(BigInt(-12), BigInt(18)),
            BigInt(6), "gcd with negative");

        expectEqual(
            BigInt::gcd(
                BigInt("12345678901234567890"),
                BigInt("9876543210")
            ),
            BigInt("90"),
            "large gcd"
        );

        expectEqual(BigInt::gcd(BigInt(0), BigInt(12)),
            BigInt(12), "gcd(0, 12)");
    });
}

void testBigIntSqrt() {
    test("BigInt - Integer Square Root", []() {
        expectEqual(BigInt::integerSqrt(BigInt(0)),
            BigInt(0), "sqrt(0)");

        expectEqual(BigInt::integerSqrt(BigInt(1)),
            BigInt(1), "sqrt(1)");

        expectEqual(BigInt::integerSqrt(BigInt(2)),
            BigInt(1), "sqrt(2)");

        expectEqual(BigInt::integerSqrt(BigInt(15)),
            BigInt(3), "sqrt(15)");

        expectEqual(BigInt::integerSqrt(BigInt(16)),
            BigInt(4), "sqrt(16)");

        expectEqual(
            BigInt::integerSqrt(BigInt("100000000000000000000")),
            BigInt("10000000000"),
            "large perfect square"
        );

        expectEqual(
            BigInt::integerSqrt(BigInt("100000000000000000001")),
            BigInt("10000000000"),
            "large non-perfect square"
        );
    });

    test("BigInt - Negative Square Root", []() {
        bool thrown = false;

        try {
            BigInt value = BigInt::integerSqrt(BigInt(-1));
        } catch (const domain_error&) {
            thrown = true;
        }

        expect(thrown, "negative sqrt should throw");
    });
}

void testBigIntRandomProperties() {
    test("BigInt - Random Arithmetic Properties", []() {
        mt19937_64 rng(20261112);

        uniform_int_distribution<long long> dist(
            -1000000000000LL,
            1000000000000LL
        );

        for (int i = 0; i < 1000; ++i) {
            BigInt a(dist(rng));
            BigInt b(dist(rng));

            // 加法交换律
            expect(a + b == b + a, "addition commutativity");

            // 乘法交换律
            expect(a * b == b * a, "multiplication commutativity");

            // 减法和加法互逆
            expect((a + b) - b == a, "addition subtraction identity");

            if (!b.isZero()) {
                BigInt q = a / b;
                BigInt r = a % b;

                expect(q * b + r == a, "division identity");

                expect(r.abs() < b.abs(),
                    "remainder magnitude");
            }
        }
    });
}

void TestBigInt() {
    passed = 0;
    failed = 0;

    testBigIntConstruction();
    testBigIntAddition();
    testBigIntSubtraction();
    testBigIntMultiplication();
    testBigIntDivision();
    testBigIntComparison();
    testBigIntGcd();
    testBigIntSqrt();
    testBigIntRandomProperties();

    cout << "\n================================\n";
    cout << "BigInt Test Results\n";
    cout << "================================\n";
    cout << "Passed: " << passed << endl;
    cout << "Failed: " << failed << endl;
    cout << "Total : " << passed + failed << endl;
}

