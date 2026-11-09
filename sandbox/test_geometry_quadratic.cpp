#include <iostream>
#include <stdexcept>
#include <string>
#include "zidian.h"

using namespace zidian;

// int passed = 0;
// int failed = 0;

// void check(bool condition, const std::string& name) {
//     if (condition) {
//         ++passed;
//         std::cout << "[PASS] " << name << '\n';
//     } else {
//         ++failed;
//         std::cout << "[FAIL] " << name << '\n';
//     }
// }

// void checkEqual(
//     const Rational& actual,
//     const Rational& expected,
//     const std::string& name
// ) {
//     check(actual == expected, name);
// }

// template<typename Exception, typename Function>
// void checkThrows(Function function, const std::string& name) {
//     try {
//         function();
//         check(false, name);
//     } catch (const Exception&) {
//         check(true, name);
//     } catch (...) {
//         check(false, name + " (unexpected exception)");
//     }
// }


void testQuadraticNumberSign() {
    QuadraticField field(Rational(2));

    // 1 + √2 > 0
    QuadraticNumber a(field, Rational(1), Rational(1));
    assert(a.sign() == 1);

    // 1 - √2 < 0
    QuadraticNumber b(field, Rational(1), Rational(-1));
    assert(b.sign() == -1);

    // -1 + √2 > 0
    QuadraticNumber c(field, Rational(-1), Rational(1));
    assert(c.sign() == 1);

    // -1 - √2 < 0
    QuadraticNumber d(field, Rational(-1), Rational(-1));
    assert(d.sign() == -1);

    // 纯有理数
    QuadraticNumber e(field, Rational(5), Rational(0));
    assert(e.sign() == 1);

    // 纯根式
    QuadraticNumber f(field, Rational(0), Rational(-3));
    assert(f.sign() == -1);

    // 零
    QuadraticNumber g(field, Rational(0), Rational(0));
    assert(g.sign() == 0);

    // 1 - 2√2 < 0
    QuadraticNumber h(field, Rational(1), Rational(-2));
    assert(h.sign() == -1);

    // 3 - 2√2 > 0
    QuadraticNumber i(field, Rational(3), Rational(-2));
    assert(i.sign() == 1);
}

void TestGeometryQuadratic() {
    QuadraticField field(Rational(2));

    // x = 1 + 2√2
    QuadraticNumber x(
        field,
        Rational(1),
        Rational(2)
    );

    // y = 3 - 4√2
    QuadraticNumber y(
        field,
        Rational(3),
        Rational(-4)
    );

    // 加法：4 - 2√2
    auto sum = x + y;

    assert(sum.getA() == Rational(4));
    assert(sum.getB() == Rational(-2));

    // 减法：-2 + 6√2
    auto difference = x - y;

    assert(difference.getA() == Rational(-2));
    assert(difference.getB() == Rational(6));

    // 乘法：
    // (1 + 2√2)(3 - 4√2)
    // = -13 + 2√2
    auto product = x * y;

    assert(product.getA() == Rational(-13));
    assert(product.getB() == Rational(2));

    // 除法：验证 (x / y) * y == x
    auto quotient = x / y;

    assert(quotient * y == x);

    // 数域合法性
    bool exceptionThrown = false;

    try {
        QuadraticField invalid(Rational(4));
    } catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    // 不同数域不允许直接运算
    QuadraticField field3(Rational(3));

    QuadraticNumber z(
        field3,
        Rational(1),
        Rational(1)
    );

    exceptionThrown = false;

    try {
        auto invalid = x + z;
        (void)invalid;
    } catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    testQuadraticNumberSign();

    std::cout << "All tests passed!" << std::endl;
}
