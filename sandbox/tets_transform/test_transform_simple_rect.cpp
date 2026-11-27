#include "zidian.h"

class TestCanvasTransformSimpleDraw : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestCanvasTransformSimpleDraw init");
        zidian::RandomUtil::setRandomSeed(0);
    }

    float rotateAngle = 0.0f;

    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        canvas->save();
        canvas->translate(100.0f , 100.0f);
        canvas->drawSimpleRect(0.0f, 0.0f, 100.0f, 100.0f, glm::vec4(1.0f, 1.0f , 0.0f, 1.0f));
        canvas->restore();

        canvas->save();
        canvas->rotate(rotateAngle, 100.0f , 100.0f);
        rotateAngle += (deltaTime * 100.0f);
        canvas->drawSimpleRect(0.0f, 0.0f, 100.0f, 100.0f, glm::vec4(1.0f, 0.0f , 0.0f, 1.0f));
        canvas->restore();
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestCanvasTransformSimpleDraw dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestCanvasTransformSimpleDrawCase(){
    zidian::AppConfig config;
    config.name = "TestCanvasTransformSimpleDraw";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.vsync = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCanvasTransformSimpleDraw>());
    app.init(config);
    app.execute();
}


