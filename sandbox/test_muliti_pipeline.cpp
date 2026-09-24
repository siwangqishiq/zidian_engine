#include "zidian.h"

class TestMultiPipeline : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestMultiPipeline init");
        auto &render = zidian::Instance->getRender();
        zidian::RandomUtil::setRandomSeed(0);

    }

    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float size = 100.0f;
        float x = 0.0f;
        canvas->drawSimpleRect(x, 0.0f, size, size, glm::vec4(1.0f, 0.0f , 0.0f, 1.0f));
        x += size;
        
        canvas->drawSimpleRect(x, 0.0f, size, size, glm::vec4(0.0f, 1.0f , 0.0f, 1.0f));
        x += size;

        canvas->drawSimpleRect(x, 0.0f, size, size, glm::vec4(0.0f, 0.0f , 1.0f, 1.0f));
        x += size;

        canvas->drawSimpleRect(x, 0.0f, size, size, glm::vec4(1.0f, 1.0f , 0.0f, 1.0f));
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestMultiPipeline dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestTestMultiPipeline(){
    zidian::AppConfig config;
    config.name = "TestMultiPipeline";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.vsync = true;

    zidian::Application app;
    app.setGameObject(std::make_shared<TestMultiPipeline>());
    app.init(config);
    app.execute();
}


