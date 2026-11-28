#include "zidian.h"

class TestCanvasTransformSimpleTriangle : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestCanvasTransformSimpleTriangle init");
        zidian::RandomUtil::setRandomSeed(0);
    }

    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        canvas->save();
        canvas->translate(100.0f, 0.0f);
        glm::vec2 p1{50.0f , 0.0f};
        glm::vec2 p2{0.0f, 100.0f};
        glm::vec2 p3{100.0f, 100.0f};
        canvas->drawSimpleTriangle(p1,p2,p3,glm::vec4(1.0f, 1.0f , 0.0f , 1.0f));
        canvas->restore();
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestCanvasTransformSimpleTriangle dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestCanvasTransformSimpleTriangleCase(){
    zidian::AppConfig config;
    config.name = "TestCanvasTransformSimpleTriangle";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.vsync = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCanvasTransformSimpleTriangle>());
    app.init(config);
    app.execute();
}


