#include <iostream>

extern void TestLogs();
extern void TestApplication();
extern void TestDraw();
extern void TestBatchDraw();
extern void TestCircleDraw();
extern void TestSpriteDraw1();
extern void TestTestMultiPipeline();

extern void TestPictureShow();

extern void TestCustomShader();

extern void TestGeometryRational();

extern void TestGeometryQuadratic();

int main() {
    // TestLogs();
    // TestApplication();
    // TestDraw();
    // TestBatchDraw();
    // TestCircleDraw();
    // TestTestMultiPipeline();
    // TestPictureShow();
    // TestSpriteDraw1();
    // TestCustomShader();

    // TestGeometryRational();

    TestGeometryQuadratic();
    return 0;
}//end main