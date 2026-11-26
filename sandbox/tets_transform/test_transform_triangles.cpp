#include "zidian.h"

class TestCanvasTransformTriangles : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestCanvasTransform init");
        zidian::RandomUtil::setRandomSeed(0);
    }

    void test1(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        // Case 1: 原始矩形
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        // Case 1: 原始矩形
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        // Case 2: 平移
        canvas->save();
        canvas->translate(200.0f, 0.0f);
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        canvas->restore();

        // Case 3: 缩放
        canvas->save();
        canvas->scale(2.0f, 2.0f);
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        canvas->restore();

        // // Case 4: 旋转 45°
        canvas->save();
        canvas->rotate(45.0f);
        canvas->drawRect(300.0f, 100.0f, 120.0f, 80.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        canvas->restore();

        // Case 5: 旋转 90°
        canvas->save();
        canvas->translate(500.0f, 100.0f);
        canvas->rotate(90.0f);
        canvas->drawRect(
            100.0f,
            100.0f,
            120.0f,
            80.0f,
            glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
        );
        canvas->restore();

        // Case 6: 平移 + 旋转
        canvas->save();
        canvas->translate(300.0f, 200.0f);
        canvas->rotate(45.0f);
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
        canvas->restore();

        // Case 7: 缩放 + 旋转
        canvas->save();
        canvas->scale(1.5f, 0.8f);
        canvas->rotate(30.0f);
        canvas->drawRect(300.0f, 300.0f, 120.0f, 80.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        canvas->restore();

        // Case 8: 复合变换（平移 + 旋转 + 缩放）
        canvas->save();
        canvas->translate(500.0f, 300.0f);
        canvas->rotate(30.0f);
        canvas->scale(1.5f, 1.5f);
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));
        canvas->restore();

        // // Case 9: 连续绘制，验证 Save / Restore 状态隔离
        canvas->save();
        canvas->translate(200.0f, 300.0f);
        canvas->rotate(45.0f);
        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));
        canvas->restore();

        canvas->drawRect(100.0f, 100.0f, 120.0f, 80.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestCanvasTransform dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestCanvasTransformTrianglesCase(){
    zidian::AppConfig config;
    config.name = "TestCanvasTransform";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.vsync = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCanvasTransformTriangles>());
    app.init(config);
    app.execute();
}


