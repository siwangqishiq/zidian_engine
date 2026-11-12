
#include <iostream>
#include <stdexcept>
#include <string>
#include <functional>
#include <vector>

#include "geometry/field_manager.h"
#include "geometry/field_embedding.h"

using namespace zidian;
using namespace std;

// ==================================================
// 测试框架
// ==================================================

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
    catch (...) {
        cout << "[FAIL] " << name << ": unknown exception" << endl;
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
// 1. 根域测试
// ==================================================

static void testRoot(FieldManager& manager) {

    test("FieldManager - Root", [&]() {

        const QuadraticField& Q = manager.getRoot();

        expect(
            Q.isBaseField(),
            "Root should be base field"
        );

        expect(
            Q.getLevel() == 0,
            "Root level should be 0"
        );

        expect(
            Q.getDimension() == 1,
            "Root dimension should be 1"
        );

        expect(
            Q.getParent() == nullptr,
            "Root should not have parent"
        );

        expect(
            manager.getFieldCount() == 1,
            "Initial field count should be 1"
        );
    });
}

// ==================================================
// 2. 第一层扩张
// Q(sqrt(2))
// ==================================================

static void testFirstExtension(
    FieldManager& manager,
    const QuadraticField& Q,
    const QuadraticField*& Q2
) {

    test("FieldManager - First Extension", [&]() {

        QuadraticNumber sqrt2(Q, Rational(2));

        Q2 = &manager.extend(Q, sqrt2);

        expect(
            Q2->getLevel() == 1,
            "Level should be 1"
        );

        expect(
            Q2->getDimension() == 2,
            "Dimension should be 2"
        );

        expect(
            Q2->getParent() == &Q,
            "Parent should be root field"
        );

        expect(
            manager.getFieldCount() == 2,
            "Field count should be 2"
        );
    });
}

// ==================================================
// 3. 重复扩张
// ==================================================

static void testDuplicateExtension(
    FieldManager& manager,
    const QuadraticField& Q,
    const QuadraticField& Q2
) {

    test("FieldManager - Duplicate Extension", [&]() {

        QuadraticNumber sqrt2(Q, Rational(2));

        const QuadraticField& anotherQ2 =
            manager.extend(Q, sqrt2);

        expect(
            &anotherQ2 == &Q2,
            "Duplicate extension should reuse existing field"
        );

        expect(
            manager.getFieldCount() == 2,
            "Duplicate extension should not increase field count"
        );
    });
}

// ==================================================
// 4. 第二层扩张
// Q(sqrt(2), sqrt(3))
// ==================================================

static void testSecondExtension(
    FieldManager& manager,
    const QuadraticField& Q2,
    const QuadraticField*& Q23
) {

    test("FieldManager - Second Extension", [&]() {

        QuadraticNumber sqrt3(Q2, Rational(3));

        Q23 = &manager.extend(Q2, sqrt3);

        expect(
            Q23->getLevel() == 2,
            "Level should be 2"
        );

        expect(
            Q23->getDimension() == 4,
            "Dimension should be 4"
        );

        expect(
            Q23->getParent() == &Q2,
            "Parent should be Q(sqrt(2))"
        );
    });
}

// ==================================================
// 5. 第三层扩张
// Q(sqrt(2), sqrt(3), sqrt(5))
// ==================================================

static void testThirdExtension(
    FieldManager& manager,
    const QuadraticField& Q23
) {

    test("FieldManager - Third Extension", [&]() {

        QuadraticNumber sqrt5(Q23, Rational(5));

        const QuadraticField& Q235 =
            manager.extend(Q23, sqrt5);

        expect(
            Q235.getLevel() == 3,
            "Level should be 3"
        );

        expect(
            Q235.getDimension() == 8,
            "Dimension should be 8"
        );

        expect(
            Q235.getParent() == &Q23,
            "Parent should be Q(sqrt(2), sqrt(3))"
        );

        expect(
            manager.getFieldCount() == 4,
            "Field count should be 4"
        );
    });
}

// ==================================================
// 6. 平方根测试
// ==================================================

static void testTrySqrt(
    FieldManager& manager,
    const QuadraticField& Q2
) {

    test("FieldManager - Try Sqrt Success", [&]() {

        QuadraticNumber value(Q2, Rational(8));
        QuadraticNumber result(Q2);

        expect(
            manager.trySqrt(Q2, value, result),
            "sqrt(8) should succeed"
        );

        expectEqual(
            result,
            {
                Rational(0),
                Rational(2)
            },
            "sqrt(8) should equal 2*sqrt(2)"
        );
    });

    test("FieldManager - Try Sqrt Failure", [&]() {

        QuadraticNumber value(Q2, Rational(3));
        QuadraticNumber result(Q2);

        expect(
            !manager.trySqrt(Q2, value, result),
            "sqrt(3) should fail in Q(sqrt(2))"
        );
    });
}

// ==================================================
// 7. 非法扩张测试
// ==================================================

static void testInvalidExtensions(
    FieldManager& manager,
    const QuadraticField& Q,
    const QuadraticField& Q2
) {

    test("FieldManager - Reject Zero", [&]() {

        QuadraticNumber zero(Q, Rational(0));

        expectThrow(
            [&]() {
                manager.extend(Q, zero);
            },
            "Zero radicand should be rejected"
        );
    });

    test("FieldManager - Reject Negative", [&]() {

        QuadraticNumber negative(Q, Rational(-2));

        expectThrow(
            [&]() {
                manager.extend(Q, negative);
            },
            "Negative radicand should be rejected"
        );
    });

    test("FieldManager - Reject Perfect Square", [&]() {

        QuadraticNumber square(Q, Rational(4));

        expectThrow(
            [&]() {
                manager.extend(Q, square);
            },
            "Perfect square should be rejected"
        );
    });

    test("FieldManager - Reject Different Field", [&]() {

        QuadraticNumber value(Q2, Rational(3));

        expectThrow(
            [&]() {
                manager.extend(Q, value);
            },
            "Radicand from another field should be rejected"
        );
    });
}

// ==================================================
// 8. 非法 trySqrt 参数测试
// ==================================================

static void testInvalidTrySqrt(
    FieldManager& manager,
    const QuadraticField& Q,
    const QuadraticField& Q2
) {

    test("FieldManager - Reject Unmanaged Field", [&]() {

        QuadraticField anotherRoot;
        QuadraticNumber value(anotherRoot, Rational(4));
        QuadraticNumber result(anotherRoot);

        expectThrow(
            [&]() {
                manager.trySqrt(anotherRoot, value, result);
            },
            "Unmanaged field should be rejected"
        );
    });

    test("FieldManager - Reject Value From Different Field", [&]() {

        QuadraticNumber value(Q, Rational(4));
        QuadraticNumber result(Q2);

        expectThrow(
            [&]() {
                manager.trySqrt(Q2, value, result);
            },
            "Value from another field should be rejected"
        );
    });

    test("FieldManager - Reject Result From Different Field", [&]() {

        QuadraticNumber value(Q2, Rational(8));
        QuadraticNumber result(Q);

        expectThrow(
            [&]() {
                manager.trySqrt(Q2, value, result);
            },
            "Result from another field should be rejected"
        );
    });
}

// ==================================================
// 9. 数域地址稳定性
// ==================================================

static void testStableAddress(
    FieldManager& manager,
    const QuadraticField& Q2
) {

    test("FieldManager - Stable Address", [&]() {

        const QuadraticField* originalAddress = &Q2;

        // 创建多个扩张节点，尝试触发 extensions_ 扩容
        for (int i = 2; i < 12; ++i) {

            QuadraticNumber value(Q2, Rational(i));

            try {
                manager.extend(Q2, value);
            }
            catch (const invalid_argument&) {
                // 某些数可能已经是当前数域中的平方
            }
        }

        expect(
            originalAddress == &Q2,
            "Field address should remain stable"
        );

        expect(
            Q2.getParent() == &manager.getRoot(),
            "Parent address should remain stable"
        );
    });
}

static void testTrySqrtOrExtend() {

    test("FieldManager - Auto Extend From Root", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value(Q, Rational(2));

        auto result = manager.trySqrtOrExtend(Q, value);

        expect(result.has_value(), "sqrt(2) should succeed");

        expect(result->extended, "Should create an extension");

        expect(
            result->field.getLevel() == 1,
            "Result field level should be 1"
        );

        expect(
            result->field.getDimension() == 2,
            "Result field dimension should be 2"
        );

        expectEqual(
            result->value,
            {
                Rational(0),
                Rational(1)
            },
            "sqrt(2) should equal the new basis element"
        );

        QuadraticNumber expected(
            result->field,
            Rational(2)
        );

        expect(
            result->value * result->value == expected,
            "Square root verification failed"
        );
    });

    test("FieldManager - Auto Extend Second Level", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        // 第一次扩张：Q(sqrt(2))
        QuadraticNumber value2(Q, Rational(2));

        auto result2 = manager.trySqrtOrExtend(Q, value2);

        expect(result2.has_value(), "sqrt(2) should succeed");

        const QuadraticField& Q2 = result2->field;

        // 第二次扩张：Q(sqrt(2), sqrt(3))
        QuadraticNumber value3(Q2, Rational(3));

        auto result3 = manager.trySqrtOrExtend(Q2, value3);

        expect(result3.has_value(), "sqrt(3) should succeed");

        expect(result3->extended, "Should create second extension");

        expect(
            result3->field.getLevel() == 2,
            "Result field level should be 2"
        );

        expect(
            result3->field.getDimension() == 4,
            "Result field dimension should be 4"
        );

        expect(
            result3->field.getParent() == &Q2,
            "Parent should be Q(sqrt(2))"
        );

        expectEqual(
            result3->value,
            {
                Rational(0),
                Rational(0),
                Rational(1),
                Rational(0)
            },
            "sqrt(3) should equal the third basis element"
        );

        QuadraticNumber expected(
            result3->field,
            Rational(3)
        );

        expect(
            result3->value * result3->value == expected,
            "Square root verification failed"
        );
    });

    test("FieldManager - Reuse Existing Extension", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value(Q, Rational(2));

        auto first = manager.trySqrtOrExtend(Q, value);

        expect(first.has_value(), "First sqrt should succeed");

        const QuadraticField* firstField = &first->field;

        auto second = manager.trySqrtOrExtend(Q, value);

        expect(second.has_value(), "Second sqrt should succeed");

        expect(
            &second->field == firstField,
            "Existing extension should be reused"
        );

        expect(
            manager.getFieldCount() == 2,
            "Duplicate extension should not increase field count"
        );
    });

    test("FieldManager - Square Root In Current Field", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value2(Q, Rational(2));

        auto first = manager.trySqrtOrExtend(Q, value2);

        expect(first.has_value(), "sqrt(2) should succeed");

        const QuadraticField& Q2 = first->field;

        // 8 在 Q(sqrt(2)) 中可以开平方
        QuadraticNumber value(Q2, Rational(8));

        auto result = manager.trySqrtOrExtend(Q2, value);

        expect(result.has_value(), "sqrt(8) should succeed");

        expect(
            !result->extended,
            "Should not create another extension"
        );

        expect(
            &result->field == &Q2,
            "Result should remain in current field"
        );

        expectEqual(
            result->value,
            {
                Rational(0),
                Rational(2)
            },
            "sqrt(8) should equal 2*sqrt(2)"
        );
    });

    test("FieldManager - Negative Square Root", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value(Q, Rational(-2));

        auto result = manager.trySqrtOrExtend(Q, value);

        expect(
            !result.has_value(),
            "Negative number should not have real square root"
        );

        expect(
            manager.getFieldCount() == 1,
            "Negative number should not create an extension"
        );
    });

    test("FieldManager - Zero Square Root", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value(Q, Rational(0));

        auto result = manager.trySqrtOrExtend(Q, value);

        expect(result.has_value(), "sqrt(0) should succeed");

        expect(
            !result->extended,
            "Zero should not create an extension"
        );

        expect(
            result->value.isZero(),
            "sqrt(0) should equal zero"
        );

        expect(
            &result->field == &Q,
            "Result should remain in root field"
        );
    });

    test("FieldManager - Perfect Square In Root Field", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value(Q, Rational(4));

        auto result = manager.trySqrtOrExtend(Q, value);

        expect(result.has_value(), "sqrt(4) should succeed");

        expect(
            !result->extended,
            "Perfect square should not create an extension"
        );

        expectEqual(
            result->value,
            { Rational(2) },
            "sqrt(4) should equal 2"
        );

        expect(
            manager.getFieldCount() == 1,
            "Perfect square should not increase field count"
        );
    });

    test("FieldManager - Auto Extend Third Level", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        // 第一层：Q(sqrt(2))
        QuadraticNumber value2(Q, Rational(2));

        auto result2 = manager.trySqrtOrExtend(Q, value2);

        expect(result2.has_value(), "sqrt(2) should succeed");
        expect(result2->extended, "First extension should be created");

        const QuadraticField& Q2 = result2->field;

        // 第二层：Q(sqrt(2), sqrt(3))
        QuadraticNumber value3(Q2, Rational(3));

        auto result3 = manager.trySqrtOrExtend(Q2, value3);

        expect(result3.has_value(), "sqrt(3) should succeed");
        expect(result3->extended, "Second extension should be created");

        const QuadraticField& Q23 = result3->field;

        // 第三层：Q(sqrt(2), sqrt(3), sqrt(5))
        QuadraticNumber value5(Q23, Rational(5));

        auto result5 = manager.trySqrtOrExtend(Q23, value5);

        expect(result5.has_value(), "sqrt(5) should succeed");
        expect(result5->extended, "Third extension should be created");

        // 检查数域结构
        expect(
            result5->field.getLevel() == 3,
            "Result field level should be 3"
        );

        expect(
            result5->field.getDimension() == 8,
            "Result field dimension should be 8"
        );

        expect(
            result5->field.getParent() == &Q23,
            "Parent should be Q(sqrt(2), sqrt(3))"
        );

        expect(
            manager.getFieldCount() == 4,
            "Manager should contain 4 fields"
        );

        // 检查第三层平方根的系数
        expectEqual(
            result5->value,
            {
                Rational(0),
                Rational(0),
                Rational(0),
                Rational(0),
                Rational(1),
                Rational(0),
                Rational(0),
                Rational(0)
            },
            "sqrt(5) should equal the fifth basis element"
        );

        // 验证平方结果
        QuadraticNumber expected(
            result5->field,
            Rational(5)
        );

        expect(
            result5->value * result5->value == expected,
            "sqrt(5) square verification failed"
        );
    });


    test("FieldManager - Equivalent Extension Sqrt8", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value2(Q, Rational(2));
        QuadraticNumber value8(Q, Rational(8));

        const QuadraticField& Q2 =
            manager.extend(Q, value2);

        const QuadraticField& Q8 =
            manager.extend(Q, value8);

        expect(
            &Q2 == &Q8,
            "Q(sqrt(2)) and Q(sqrt(8)) should be identical"
        );

        expect(
            manager.getFieldCount() == 2,
            "Equivalent extension should not create a new field"
        );
    });

    test("FieldManager - Equivalent Extension Sqrt18", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value2(Q, Rational(2));
        QuadraticNumber value18(Q, Rational(18));

        const QuadraticField& Q2 =
            manager.extend(Q, value2);

        const QuadraticField& Q18 =
            manager.extend(Q, value18);

        expect(
            &Q2 == &Q18,
            "Q(sqrt(2)) and Q(sqrt(18)) should be identical"
        );

        expect(
            manager.getFieldCount() == 2,
            "Equivalent extension should not create a new field"
        );
    });

    test("FieldManager - Equivalent Sqrt Result", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value2(Q, Rational(2));

        auto first =
            manager.trySqrtOrExtend(Q, value2);

        expect(first.has_value(), "sqrt(2) should succeed");

        const QuadraticField& Q2 = first->field;

        QuadraticNumber value8(Q, Rational(8));

        auto result =
            manager.trySqrtOrExtend(Q, value8);

        expect(result.has_value(), "sqrt(8) should succeed");

        expect(
            !result->extended,
            "Equivalent extension should be reused"
        );

        expect(
            &result->field == &Q2,
            "Result should belong to Q(sqrt(2))"
        );

        expectEqual(
            result->value,
            {
                Rational(0),
                Rational(2)
            },
            "sqrt(8) should equal 2*sqrt(2)"
        );

        QuadraticNumber expected(Q2, Rational(8));

        expect(
            result->value * result->value == expected,
            "sqrt(8) verification failed"
        );

        expect(
            manager.getFieldCount() == 2,
            "No redundant extension should be created"
        );
    });

    test("FieldManager - Equivalent Sqrt18 Result", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value2(Q, Rational(2));

        auto first =
            manager.trySqrtOrExtend(Q, value2);

        expect(first.has_value(), "sqrt(2) should succeed");

        const QuadraticField& Q2 = first->field;

        QuadraticNumber value18(Q, Rational(18));

        auto result =
            manager.trySqrtOrExtend(Q, value18);

        expect(result.has_value(), "sqrt(18) should succeed");

        expect(
            !result->extended,
            "Equivalent extension should be reused"
        );

        expect(
            &result->field == &Q2,
            "Result should belong to Q(sqrt(2))"
        );

        expectEqual(
            result->value,
            {
                Rational(0),
                Rational(3)
            },
            "sqrt(18) should equal 3*sqrt(2)"
        );

        QuadraticNumber expected(Q2, Rational(18));

        expect(
            result->value * result->value == expected,
            "sqrt(18) verification failed"
        );
    });

    test("FieldManager - Reject Non Equivalent Extension", [&]() {

        FieldManager manager;

        const QuadraticField& Q = manager.getRoot();

        QuadraticNumber value2(Q, Rational(2));
        QuadraticNumber value3(Q, Rational(3));

        const QuadraticField& Q2 =
            manager.extend(Q, value2);

        const QuadraticField& Q3 =
            manager.extend(Q, value3);

        expect(
            &Q2 != &Q3,
            "Q(sqrt(2)) and Q(sqrt(3)) should be different"
        );

        expect(
            manager.getFieldCount() == 3,
            "Two different extensions should be created"
        );
    });

}

void testEquivalentExtensionOrder() {
    test("FieldManager: equivalent extension order",[&](){
        FieldManager manager;

        const QuadraticField& root =
            manager.getRoot();

        // K1 = Q(sqrt(2))
        QuadraticNumber two(root, Rational(2));

        const QuadraticField& fieldA =
            manager.extend(root, two);

        // L1 = Q(sqrt(2))(sqrt(3))
        QuadraticNumber threeA(
            fieldA,
            std::vector<Rational>{
                Rational(3),
                Rational(0)
            }
        );

        const QuadraticField& extensionA =
            manager.extend(fieldA, threeA);

        // K2 = Q(sqrt(3))
        QuadraticNumber three(root, Rational(3));

        const QuadraticField& fieldB =
            manager.extend(root, three);

        // L2 = Q(sqrt(3))(sqrt(2))
        QuadraticNumber twoB(
            fieldB,
            std::vector<Rational>{
                Rational(2),
                Rational(0)
            }
        );

        const QuadraticField& extensionB =
            manager.extend(fieldB, twoB);

        // 两个扩张应复用同一个数域对象。
        expect(
            &extensionA == &extensionB,
            "Different extension orders should reuse the same field"
        );

        expect(
            manager.getFieldCount() == std::size_t(4),
            "Equivalent extension should not create a new field"
        );
    });
}

void testSqrtFromEquivalentExtension() {
    test("FieldManager: sqrt from equivalent extension",[&](){
        FieldManager manager;

        const QuadraticField& root =
            manager.getRoot();

        // K1 = Q(sqrt(2))
        QuadraticNumber two(root, Rational(2));

        const QuadraticField& fieldA =
            manager.extend(root, two);

        // L1 = Q(sqrt(2), sqrt(3))
        QuadraticNumber threeA(
            fieldA,
            std::vector<Rational>{
                Rational(3),
                Rational(0)
            }
        );

        const QuadraticField& extensionA =
            manager.extend(fieldA, threeA);

        // K2 = Q(sqrt(3))
        QuadraticNumber three(root, Rational(3));

        const QuadraticField& fieldB =
            manager.extend(root, three);

        // 在 Q(sqrt(3)) 中请求 sqrt(2)。
        QuadraticNumber twoB(
            fieldB,
            std::vector<Rational>{
                Rational(2),
                Rational(0)
            }
        );

        auto result =
            manager.trySqrtOrExtend(fieldB, twoB);

        expect(
            result.has_value(),
            "Should find square root in existing extension"
        );

        if (!result.has_value()) {
            return;
        }

        expect(
            &result->field == &extensionA,
            "Should reuse existing extension field"
        );

        expect(
            !result->extended,
            "Should not create a new extension"
        );

        // 验证 sqrt(2)^2 == 2。
        const QuadraticNumber expected(
            extensionA,
            std::vector<Rational>{
                Rational(2),
                Rational(0),
                Rational(0),
                Rational(0)
            }
        );

        expect(
            result->value * result->value == expected,
            "Converted square root should be mathematically correct"
        );
    });
}


void testEquivalentGeneralQuadraticExtension() {
    test("FieldManager: equivalent general quadratic extension", [&]() {
        FieldManager manager;

        const QuadraticField& root = manager.getRoot();

        // 路径 A:
        // L1 = Q(sqrt(2))(sqrt(3))

        QuadraticNumber two(root, Rational(2));
        const QuadraticField& field2 =
            manager.extend(root, two);

        QuadraticNumber three(
            field2,
            std::vector<Rational>{
                Rational(3),
                Rational(0)
            }
        );

        const QuadraticField& extensionA =
            manager.extend(field2, three);

        // 路径 B:
        // K = Q(sqrt(6))

        QuadraticNumber six(root, Rational(6));
        const QuadraticField& field6 =
            manager.extend(root, six);

        // radicand = 5 + 2 * sqrt(6)
        QuadraticNumber radicand(
            field6,
            std::vector<Rational>{
                Rational(5),
                Rational(2)
            }
        );

        // L2 = Q(sqrt(6))(sqrt(5 + 2 * sqrt(6)))
        const QuadraticField& extensionB =
            manager.extend(field6, radicand);

        expect(
            &extensionA == &extensionB,
            "General quadratic extension should reuse existing field"
        );

        expect(
            manager.getFieldCount() == std::size_t(4),
            "Equivalent general extension should not create a new field"
        );
    });
}


void testSqrtFromGeneralEquivalentExtension() {
    test("FieldManager: sqrt from general equivalent extension", [&]() {
        FieldManager manager;

        const QuadraticField& root = manager.getRoot();

        // 已有域：
        // L = Q(sqrt(2))(sqrt(3))

        QuadraticNumber two(root, Rational(2));
        const QuadraticField& field2 =
            manager.extend(root, two);

        QuadraticNumber three(
            field2,
            std::vector<Rational>{
                Rational(3),
                Rational(0)
            }
        );

        const QuadraticField& extension =
            manager.extend(field2, three);

        // 另一个父域：
        // K = Q(sqrt(6))

        QuadraticNumber six(root, Rational(6));
        const QuadraticField& field6 =
            manager.extend(root, six);

        // a = 5 + 2 * sqrt(6)

        QuadraticNumber radicand(
            field6,
            std::vector<Rational>{
                Rational(5),
                Rational(2)
            }
        );

        // 请求 sqrt(a)
        auto result =
            manager.trySqrtOrExtend(field6, radicand);

        expect(
            result.has_value(),
            "Should find square root in equivalent extension"
        );

        if (!result.has_value()) {
            return;
        }

        expect(
            &result->field == &extension,
            "Should reuse the existing extension"
        );

        expect(
            !result->extended,
            "Should not create a new extension"
        );

        // L 的基底顺序：
        //
        // 1, sqrt(2), sqrt(3), sqrt(6)
        //
        // sqrt(2) + sqrt(3) = [0, 1, 1, 0]

        QuadraticNumber expectedRoot(
            extension,
            std::vector<Rational>{
                Rational(0),
                Rational(1),
                Rational(1),
                Rational(0)
            }
        );

        expect(
            result->value == expectedRoot,
            "Square root should equal sqrt(2) + sqrt(3)"
        );

        // 验证平方结果：
        //
        // (sqrt(2) + sqrt(3))^2
        // = 5 + 2 * sqrt(6)

        QuadraticNumber expectedRadicand(
            extension,
            std::vector<Rational>{
                Rational(5),
                Rational(0),
                Rational(0),
                Rational(2)
            }
        );

        expect(
            result->value * result->value == expectedRadicand,
            "Square root should produce the correct radicand"
        );
    });
}


void testEquivalentNestedRadicalExtension() {
    test("FieldManager: equivalent nested radical extension", [&]() {
        FieldManager manager;

        const QuadraticField& root = manager.getRoot();

        // --------------------------------------------
        // 路径 A:
        // L1 = Q(sqrt(2))(sqrt(3))
        // --------------------------------------------

        QuadraticNumber two(root, Rational(2));

        const QuadraticField& field2 =
            manager.extend(root, two);

        QuadraticNumber threeA(
            field2,
            std::vector<Rational>{
                Rational(3),
                Rational(0)
            }
        );

        const QuadraticField& extensionA =
            manager.extend(field2, threeA);

        // --------------------------------------------
        // 路径 B:
        // K = Q(sqrt(3))
        // L2 = K(sqrt(2 + sqrt(3)))
        // --------------------------------------------

        QuadraticNumber three(root, Rational(3));

        const QuadraticField& field3 =
            manager.extend(root, three);

        QuadraticNumber nestedRadicand(
            field3,
            std::vector<Rational>{
                Rational(2),
                Rational(1)
            }
        );

        const QuadraticField& extensionB =
            manager.extend(field3, nestedRadicand);

        expect(
            &extensionA == &extensionB,
            "Nested radical extension should reuse existing field"
        );

        expect(
            manager.getFieldCount() == std::size_t(4),
            "Equivalent nested extension should not create a new field"
        );
    });
}



void testSqrtFromNestedEquivalentExtension() {
    test("FieldManager: sqrt from nested equivalent extension", [&]() {
        FieldManager manager;

        const QuadraticField& root = manager.getRoot();

        // --------------------------------------------
        // 路径 A:
        // L = Q(sqrt(2))(sqrt(3))
        // --------------------------------------------

        QuadraticNumber two(root, Rational(2));

        const QuadraticField& field2 =
            manager.extend(root, two);

        QuadraticNumber threeA(
            field2,
            std::vector<Rational>{
                Rational(3),
                Rational(0)
            }
        );

        const QuadraticField& extension =
            manager.extend(field2, threeA);

        // --------------------------------------------
        // 路径 B:
        // K = Q(sqrt(3))
        // --------------------------------------------

        QuadraticNumber three(root, Rational(3));

        const QuadraticField& field3 =
            manager.extend(root, three);

        // a = 2 + sqrt(3)

        QuadraticNumber radicand(
            field3,
            std::vector<Rational>{
                Rational(2),
                Rational(1)
            }
        );

        // 请求 sqrt(2 + sqrt(3))
        auto result =
            manager.trySqrtOrExtend(field3, radicand);

        expect(
            result.has_value(),
            "Should find nested square root in equivalent extension"
        );

        if (!result.has_value()) {
            return;
        }

        expect(
            &result->field == &extension,
            "Should reuse the existing extension"
        );

        expect(
            !result->extended,
            "Should not create a new extension"
        );

        // --------------------------------------------
        // 预期结果：
        //
        // sqrt(2 + sqrt(3))
        // = (sqrt(2) + sqrt(6)) / 2
        //
        // 基底：
        // 1, sqrt(2), sqrt(3), sqrt(6)
        // --------------------------------------------

        QuadraticNumber expectedRoot(
            extension,
            std::vector<Rational>{
                Rational(0),
                Rational(BigInt(1), BigInt(2)),
                Rational(0),
                Rational(BigInt(1), BigInt(2))
            }
        );

        expect(
            result->value == expectedRoot,
            "Nested square root should have the expected coefficients"
        );

        // --------------------------------------------
        // 验证平方结果：
        //
        // ((sqrt(2) + sqrt(6)) / 2)^2
        // = 2 + sqrt(3)
        // --------------------------------------------

        QuadraticNumber expectedRadicand(
            extension,
            std::vector<Rational>{
                Rational(2),
                Rational(0),
                Rational(1),
                Rational(0)
            }
        );

        expect(
            result->value * result->value == expectedRadicand,
            "Nested square root should produce the correct radicand"
        );
    });
}


void testEquivalentThreeLevelExtensionOrder() {
    test("FieldManager: equivalent three-level extension order", [&]() {
        FieldManager manager;

        const QuadraticField& root = manager.getRoot();

        // 公共第一层：Q(sqrt(2))
        QuadraticNumber two(root, Rational(2));
        const QuadraticField& field2 = manager.extend(root, two);

        // 路径 A：先 sqrt(3)
        QuadraticNumber threeA(
            field2,
            std::vector<Rational>{
                Rational(3), Rational(0)
            }
        );

        const QuadraticField& field23 =
            manager.extend(field2, threeA);

        // 2 + sqrt(2)，属于 Q(sqrt(2))
        QuadraticNumber nestedRadicand(
            field2,
            std::vector<Rational>{
                Rational(2), Rational(1)
            }
        );

        // 将 radicand 转换到 Q(sqrt(2), sqrt(3))
        QuadraticNumber nestedRadicandA(
            field23,
            std::vector<Rational>{
                Rational(2),
                Rational(1),
                Rational(0),
                Rational(0)
            }
        );

        const QuadraticField& fieldA =
            manager.extend(field23, nestedRadicandA);

        // 路径 B：先 sqrt(2 + sqrt(2))
        const QuadraticField& fieldNested =
            manager.extend(field2, nestedRadicand);

        // 再扩张 sqrt(3)
        QuadraticNumber threeB(
            fieldNested,
            std::vector<Rational>{
                Rational(3),
                Rational(0),
                Rational(0),
                Rational(0)
            }
        );

        const QuadraticField& fieldB =
            manager.extend(fieldNested, threeB);

        expect(
            &fieldA == &fieldB,
            "Three-level extension orders should reuse the same field"
        );

        expect(
            manager.getFieldCount() == std::size_t(5),
            "Should contain root and four extension nodes"
        );
    });
}


struct EmbeddingTestFields {
    std::unique_ptr<FieldManager> manager;

    const QuadraticField* field = nullptr;
};

EmbeddingTestFields createPathA() {
    EmbeddingTestFields result;
    result.manager = std::make_unique<FieldManager>();

    const QuadraticField& root = result.manager->getRoot();

    // Q(sqrt(2))
    QuadraticNumber two(root, Rational(2));
    const QuadraticField& field2 =
        result.manager->extend(root, two);

    // Q(sqrt(2), sqrt(3))
    QuadraticNumber three(
        field2,
        std::vector<Rational>{
            Rational(3), Rational(0)
        }
    );

    const QuadraticField& field23 =
        result.manager->extend(field2, three);

    // 2 + sqrt(2)
    // 在 field23 中的基底为：
    // 1, sqrt(2), sqrt(3), sqrt(6)
    QuadraticNumber nestedRadicand(
        field23,
        std::vector<Rational>{
            Rational(2),
            Rational(1),
            Rational(0),
            Rational(0)
        }
    );

    result.field = &result.manager->extend(
        field23,
        nestedRadicand
    );

    return result;
}

EmbeddingTestFields createPathB() {
    EmbeddingTestFields result;
    result.manager = std::make_unique<FieldManager>();

    const QuadraticField& root = result.manager->getRoot();

    // Q(sqrt(2))
    QuadraticNumber two(root, Rational(2));
    const QuadraticField& field2 =
        result.manager->extend(root, two);

    // Q(sqrt(2), sqrt(2 + sqrt(2)))
    QuadraticNumber nestedRadicand(
        field2,
        std::vector<Rational>{
            Rational(2),
            Rational(1)
        }
    );

    const QuadraticField& fieldNested =
        result.manager->extend(field2, nestedRadicand);

    // Q(sqrt(2), sqrt(2 + sqrt(2)), sqrt(3))
    QuadraticNumber three(
        fieldNested,
        std::vector<Rational>{
            Rational(3),
            Rational(0),
            Rational(0),
            Rational(0)
        }
    );

    result.field = &result.manager->extend(
        fieldNested,
        three
    );

    return result;
}


void testFieldEmbeddingIdentityAndRoundTrip() {
    test("FieldEmbedding: identity and round trip", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding forward(fieldA, fieldB);
        FieldEmbedding backward(fieldB, fieldA);

        QuadraticNumber zeroA(
            fieldA,
            std::vector<Rational>(8, Rational(0))
        );

        QuadraticNumber oneA(
            fieldA,
            std::vector<Rational>{
                Rational(1),
                Rational(0), Rational(0), Rational(0),
                Rational(0), Rational(0), Rational(0), Rational(0)
            }
        );

        QuadraticNumber zeroB(
            fieldB,
            std::vector<Rational>(8, Rational(0))
        );

        QuadraticNumber oneB(
            fieldB,
            std::vector<Rational>{
                Rational(1),
                Rational(0), Rational(0), Rational(0),
                Rational(0), Rational(0), Rational(0), Rational(0)
            }
        );

        expect(
            forward.convert(zeroA) == zeroB,
            "Embedding should preserve zero"
        );

        expect(
            forward.convert(oneA) == oneB,
            "Embedding should preserve one"
        );

        QuadraticNumber a(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber converted = forward.convert(a);
        QuadraticNumber restored = backward.convert(converted);

        expect(
            restored == a,
            "Round trip conversion should preserve the original value"
        );
    });
}


void testFieldEmbeddingArithmetic() {
    test("FieldEmbedding: arithmetic preservation", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        QuadraticNumber a(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber b(
            fieldA,
            std::vector<Rational>{
                Rational(2), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(1),
                Rational(1), Rational(2)
            }
        );

        QuadraticNumber mappedA = embedding.convert(a);
        QuadraticNumber mappedB = embedding.convert(b);

        // 加法保持性
        expect(
            embedding.convert(a + b) == mappedA + mappedB,
            "Embedding should preserve addition"
        );

        // 减法保持性
        expect(
            embedding.convert(a - b) == mappedA - mappedB,
            "Embedding should preserve subtraction"
        );

        // 乘法保持性
        expect(
            embedding.convert(a * b) == mappedA * mappedB,
            "Embedding should preserve multiplication"
        );
    });
}


void testFieldEmbeddingReverseArithmetic() {
    test("FieldEmbedding: reverse arithmetic preservation", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldB, fieldA);

        QuadraticNumber a(
            fieldB,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber b(
            fieldB,
            std::vector<Rational>{
                Rational(2), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(1),
                Rational(1), Rational(2)
            }
        );

        QuadraticNumber mappedA = embedding.convert(a);
        QuadraticNumber mappedB = embedding.convert(b);

        expect(
            embedding.convert(a + b) == mappedA + mappedB,
            "Reverse embedding should preserve addition"
        );

        expect(
            embedding.convert(a - b) == mappedA - mappedB,
            "Reverse embedding should preserve subtraction"
        );

        expect(
            embedding.convert(a * b) == mappedA * mappedB,
            "Reverse embedding should preserve multiplication"
        );
    });
}


void testFieldEmbeddingDivision() {
    test("FieldEmbedding: division preservation", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        QuadraticNumber a(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber b(
            fieldA,
            std::vector<Rational>{
                Rational(2), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(1),
                Rational(1), Rational(2)
            }
        );

        expect(
            b != QuadraticNumber(
                fieldA,
                std::vector<Rational>(8, Rational(0))
            ),
            "Divisor should not be zero"
        );

        QuadraticNumber mappedA = embedding.convert(a);
        QuadraticNumber mappedB = embedding.convert(b);

        expect(
            embedding.convert(a / b) == mappedA / mappedB,
            "Embedding should preserve division"
        );
    });
}


void testFieldEmbeddingInjectivity() {
    test("FieldEmbedding: injectivity", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        QuadraticNumber a(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber b(
            fieldA,
            std::vector<Rational>{
                Rational(2), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(1),
                Rational(1), Rational(2)
            }
        );

        expect(a != b, "Original values should be different");

        QuadraticNumber mappedA = embedding.convert(a);
        QuadraticNumber mappedB = embedding.convert(b);

        expect(
            mappedA != mappedB,
            "Embedding should map different values to different values"
        );
    });
}


void testFieldEmbeddingSignPreservation() {
    test("FieldEmbedding: sign preservation", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        QuadraticNumber positive(
            fieldA,
            Rational(5)
        );

        QuadraticNumber negative(
            fieldA,
            Rational(-5)
        );

        QuadraticNumber zero(fieldA);

        QuadraticNumber mappedPositive =
            embedding.convert(positive);

        QuadraticNumber mappedNegative =
            embedding.convert(negative);

        QuadraticNumber mappedZero =
            embedding.convert(zero);

        expect(
            mappedPositive.isPositive(),
            "Embedding should preserve positive values"
        );

        expect(
            mappedNegative.isNegative(),
            "Embedding should preserve negative values"
        );

        expect(
            mappedZero.isZero(),
            "Embedding should preserve zero"
        );

        expect(
            positive.sign() == mappedPositive.sign(),
            "Positive sign should be preserved"
        );

        expect(
            negative.sign() == mappedNegative.sign(),
            "Negative sign should be preserved"
        );

        expect(
            zero.sign() == mappedZero.sign(),
            "Zero sign should be preserved"
        );
    });
}


void testFieldEmbeddingOrderPreservation() {
    test("FieldEmbedding: order preservation", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        QuadraticNumber a(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber positiveDifference(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        QuadraticNumber b = a + positiveDifference;

        QuadraticNumber mappedA = embedding.convert(a);
        QuadraticNumber mappedB = embedding.convert(b);

        expect(
            a < b,
            "Original values should satisfy a < b"
        );

        expect(
            mappedA < mappedB,
            "Embedding should preserve a < b"
        );

        expect(
            embedding.convert(b - a).isPositive(),
            "Embedding should preserve positive differences"
        );

        expect(
            (mappedB - mappedA).isPositive(),
            "Mapped difference should remain positive"
        );
    });
}


void testFieldEmbeddingEqualityPreservation() {
    test("FieldEmbedding: equality preservation", [&]() {
        auto pathA = createPathA();
        auto pathB = createPathB();

        const auto& fieldA = *pathA.field;
        const auto& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        QuadraticNumber a(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(0), Rational(1),
                Rational(1), Rational(0),
                Rational(2), Rational(1)
            }
        );

        QuadraticNumber b(
            fieldA,
            std::vector<Rational>{
                Rational(2), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(1),
                Rational(1), Rational(2)
            }
        );

        QuadraticNumber c = a + b;

        QuadraticNumber mappedA = embedding.convert(a);
        QuadraticNumber mappedB = embedding.convert(b);
        QuadraticNumber mappedC = embedding.convert(c);

        expect(
            embedding.convert(a) == mappedA,
            "Repeated conversion should produce equal values"
        );

        expect(
            embedding.convert(a + b) == mappedC,
            "Equal expressions should map to equal values"
        );

        expect(
            mappedC == mappedA + mappedB,
            "Equality should be preserved under addition"
        );

        expect(
            embedding.convert(a - a).isZero(),
            "Mapping a - a should produce zero"
        );
    });
}

static void verifyEmbeddingGeneratorSquare(
    const FieldEmbedding& embedding,
    const QuadraticField& source,
    const QuadraticField& extension
) {
    const std::size_t dimension = extension.getDimension();
    const std::size_t parentDimension =
        source.getParent()->getDimension();

    // 当前扩张的生成元 sqrt(d)：
    // 在 A + B*sqrt(d) 的系数布局中，
    // B 的第一个系数位于 parentDimension。
    std::vector<Rational> generatorCoefficients(
        dimension,
        Rational(0)
    );

    generatorCoefficients[parentDimension] = Rational(1);

    QuadraticNumber generator(
        extension,
        generatorCoefficients
    );

    // radicand 属于 parent 域。
    // 将它的系数复制到 extension 的前半部分，
    // 从而构造一个属于 extension 的相同元素。
    const QuadraticNumber* radicand =
        source.getRadicand();

    expect(
        radicand != nullptr,
        "Extension radicand should not be null"
    );

    expect(
        radicand->getCoefficients().size() == parentDimension,
        "Radicand coefficient dimension mismatch"
    );

    std::vector<Rational> radicandCoefficients(
        dimension,
        Rational(0)
    );

    for (std::size_t i = 0; i < parentDimension; ++i) {
        radicandCoefficients[i] =
            radicand->getCoefficients()[i];
    }

    QuadraticNumber liftedRadicand(
        extension,
        radicandCoefficients
    );

    QuadraticNumber mappedGenerator =
        embedding.convert(generator);

    QuadraticNumber mappedRadicand =
        embedding.convert(liftedRadicand);

    expect(
        mappedGenerator * mappedGenerator == mappedRadicand,
        "Mapped generator square should equal mapped radicand"
    );
}

void testFieldEmbeddingTwoLevelGenerators() {
    test("FieldEmbedding: two-level generator mapping", [&]() {

        auto pathA = createPathA();
        auto pathB = createPathB();

        const QuadraticField& fieldA = *pathA.field;
        const QuadraticField& fieldB = *pathB.field;

        // 两棵不同扩张树之间建立映射。
        FieldEmbedding embedding(fieldA, fieldB);

        const QuadraticField* current = &fieldA;

        // 逐层验证：
        // Q(sqrt(2))
        // Q(sqrt(2), sqrt(3))
        for (int i = 0; i < 2; ++i) {

            const QuadraticField* parent =
                current->getParent();

            expect(
                parent != nullptr,
                "Parent field should not be null"
            );

            verifyEmbeddingGeneratorSquare(
                embedding,
                *current,
                fieldA
            );

            current = parent;
        }
    });
}



void testFieldEmbeddingThreeLevelGenerators() {
    test("FieldEmbedding: three-level generator mapping", [&]() {

        auto pathA = createPathA();
        auto pathB = createPathB();

        const QuadraticField& fieldA = *pathA.field;
        const QuadraticField& fieldB = *pathB.field;

        FieldEmbedding embedding(fieldA, fieldB);

        // 最终域基底：
        // 1, sqrt(2), sqrt(3), sqrt(6),
        // sqrt(2 + sqrt(2)), ...
        
        QuadraticNumber sqrt2(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(1),
                Rational(0), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        QuadraticNumber sqrt3(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        QuadraticNumber sqrtNested(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(0),
                Rational(0), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(0)
            }
        );

        // 构造最终源域中的 radicand。
        QuadraticNumber twoPlusSqrt2 =
            QuadraticNumber(
                fieldA,
                std::vector<Rational>{
                    Rational(2), Rational(1),
                    Rational(0), Rational(0),
                    Rational(0), Rational(0),
                    Rational(0), Rational(0)
                }
            );

        QuadraticNumber three(fieldA, Rational(3));
        QuadraticNumber two(fieldA, Rational(2));

        // 第一层：sqrt(2)^2 = 2
        expect(
            embedding.convert(sqrt2) * embedding.convert(sqrt2)
                == embedding.convert(two),
            "Mapped sqrt(2) square should equal mapped 2"
        );

        // 第二层：sqrt(3)^2 = 3
        expect(
            embedding.convert(sqrt3) * embedding.convert(sqrt3)
                == embedding.convert(three),
            "Mapped sqrt(3) square should equal mapped 3"
        );

        // 第三层：
        // sqrt(2 + sqrt(2))^2 = 2 + sqrt(2)
        expect(
            embedding.convert(sqrtNested) *
            embedding.convert(sqrtNested)
                == embedding.convert(twoPlusSqrt2),
            "Mapped nested generator square should equal mapped radicand"
        );
    });
}


void testFieldEmbeddingComposition() {
    test("FieldEmbedding: composition", [&]() {

        auto pathA = createPathA();
        auto pathB = createPathB();
        auto pathC = createPathA();

        const QuadraticField& fieldA = *pathA.field;
        const QuadraticField& fieldB = *pathB.field;
        const QuadraticField& fieldC = *pathC.field;

        FieldEmbedding embeddingAB(fieldA, fieldB);
        FieldEmbedding embeddingBC(fieldB, fieldC);
        FieldEmbedding embeddingAC(fieldA, fieldC);

        // 在 A 域中构造若干测试元素。
        std::vector<QuadraticNumber> values;

        // 1
        values.emplace_back(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        // sqrt(2)
        values.emplace_back(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(1),
                Rational(0), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        // sqrt(3)
        values.emplace_back(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        // sqrt(6)
        values.emplace_back(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(0),
                Rational(0), Rational(1),
                Rational(0), Rational(0),
                Rational(0), Rational(0)
            }
        );

        // sqrt(2 + sqrt(2))
        values.emplace_back(
            fieldA,
            std::vector<Rational>{
                Rational(0), Rational(0),
                Rational(0), Rational(0),
                Rational(1), Rational(0),
                Rational(0), Rational(0)
            }
        );

        // 一个包含多个基底分量的一般元素。
        values.emplace_back(
            fieldA,
            std::vector<Rational>{
                Rational(1), Rational(2),
                Rational(3), Rational(4),
                Rational(5), Rational(6),
                Rational(7), Rational(8)
            }
        );

        for (const auto& value : values) {
            QuadraticNumber direct =
                embeddingAC.convert(value);

            QuadraticNumber composed =
                embeddingBC.convert(
                    embeddingAB.convert(value)
                );

            expect(
                direct == composed,
                "Direct embedding should equal composed embedding"
            );
        }
    });
}


void testFieldEmbeddingRejectInvalidTargets() {
    test("FieldEmbedding: reject invalid target fields", [&]() {

        auto pathA = createPathA();

        const QuadraticField& source = *pathA.field;

        // 目标一：源域的直接父域。
        // 缺少第三层生成元 sqrt(2 + sqrt(2))。
        const QuadraticField& parent = *source.getParent();

        expectThrow(
            [&]() {
                FieldEmbedding invalid(source, parent);
            },
            "Embedding into parent field should fail"
        );

        // 目标二：根域 Q。
        // 缺少源域中的所有非平凡扩张生成元。
        const QuadraticField* root = &source;

        while (!root->isBaseField()) {
            root = root->getParent();
        }

        expectThrow(
            [&]() {
                FieldEmbedding invalid(source, *root);
            },
            "Embedding into base field should fail"
        );
    });
}



// ==================================================
// 测试入口
// ==================================================

int TestGeometryFieldManager() {

    passed = 0;
    failed = 0;

    FieldManager manager;

    const QuadraticField& Q = manager.getRoot();

    const QuadraticField* Q2 = nullptr;
    const QuadraticField* Q23 = nullptr;

    // --------------------------------------------------
    // 基础数域测试
    // --------------------------------------------------

    testRoot(manager);

    // --------------------------------------------------
    // 扩张树测试
    // --------------------------------------------------

    testFirstExtension(manager, Q, Q2);
    testDuplicateExtension(manager, Q, *Q2);
    testSecondExtension(manager, *Q2, Q23);
    testThirdExtension(manager, *Q23);

    // --------------------------------------------------
    // 平方根测试
    // --------------------------------------------------

    testTrySqrt(manager, *Q2);

    // --------------------------------------------------
    // 异常测试
    // --------------------------------------------------

    testInvalidExtensions(manager, Q, *Q2);
    testInvalidTrySqrt(manager, Q, *Q2);

    // --------------------------------------------------
    // 生命周期测试
    // --------------------------------------------------

    testStableAddress(manager, *Q2);
    testTrySqrtOrExtend();
    testEquivalentExtensionOrder();
    testSqrtFromEquivalentExtension();
    testEquivalentGeneralQuadraticExtension();
    testSqrtFromGeneralEquivalentExtension();
    testEquivalentNestedRadicalExtension();
    testSqrtFromNestedEquivalentExtension();
    testEquivalentThreeLevelExtensionOrder();
    testFieldEmbeddingIdentityAndRoundTrip();
    testFieldEmbeddingArithmetic();
    testFieldEmbeddingReverseArithmetic();

    testFieldEmbeddingDivision();
    testFieldEmbeddingInjectivity();

    testFieldEmbeddingSignPreservation();
    testFieldEmbeddingOrderPreservation();    
    testFieldEmbeddingEqualityPreservation();

    testFieldEmbeddingTwoLevelGenerators();
    testFieldEmbeddingThreeLevelGenerators();
    testFieldEmbeddingComposition();

    testFieldEmbeddingRejectInvalidTargets();
    // --------------------------------------------------
    // 输出测试结果
    // --------------------------------------------------

    cout << endl;
    cout << "================================" << endl;
    cout << "FieldManager Test Results" << endl;
    cout << "================================" << endl;

    cout << "Passed: " << passed << endl;
    cout << "Failed: " << failed << endl;
    cout << "Total : " << passed + failed << endl;

    cout << "Managed Fields: "
         << manager.getFieldCount() << endl;

    return failed == 0 ? 0 : 1;
}
