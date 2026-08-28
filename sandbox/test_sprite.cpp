#include "zidian.h"

class TestSpriteDraw : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestSprite init");
        auto &render = zidian::Instance->getRender();
        zidian::RandomUtil::setRandomSeed(0);

        std::shared_ptr<zidian::Image> image1 = render->textureManager->loadImageByPath("images/test1.jpg");
        std::shared_ptr<zidian::Image> image2 = render->textureManager->loadImageByPath("images/test2.jpg");

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
    config.isRecordLog = true;

    zidian::Application app;
    app.setGameObject(std::make_shared<TestSpriteDraw>());
    app.init(config);
    app.execute();
}


