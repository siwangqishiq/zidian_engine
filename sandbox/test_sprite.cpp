#include "zidian.h"

class TestSpriteDraw : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestSprite init");
        auto &render = zidian::Instance->getRender();
        zidian::RandomUtil::setRandomSeed(0);
    }

    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();


    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};


void TestSpriteDraw1(){
    zidian::AppConfig config;
    config.name = "TestSpriteDraw";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    config.vsync = true;
    config.isFullScreen = false;

    zidian::Application app;
    app.setGameObject(std::make_shared<TestSpriteDraw>());
    app.init(config);
    app.execute();
}




