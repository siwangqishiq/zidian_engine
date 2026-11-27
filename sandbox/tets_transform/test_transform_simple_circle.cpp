#include "zidian.h"

class TestCanvasTransformSimpleCircle : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestCanvasTransformSimpleCircle init");
        zidian::RandomUtil::setRandomSeed(0);
    }

    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float radius = 100.0f;
        glm::vec2 center(radius, radius);

        canvas->save();
        canvas->translate(-radius/2.0f, -radius/2.0f);
        canvas->drawSimpleCircle(center, radius, glm::vec4(1.0f, 1.0f , 0.0f , 1.0f));
        canvas->restore();
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestCanvasTransformSimpleCircle dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestCanvasTransformSimpleCircleCase(){
    zidian::AppConfig config;
    config.name = "TestCanvasTransformSimpleCircle";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.vsync = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCanvasTransformSimpleCircle>());
    app.init(config);
    app.execute();
}


