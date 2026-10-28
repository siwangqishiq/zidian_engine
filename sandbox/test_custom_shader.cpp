#include "zidian.h"

class TestCustomShaderCase : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestCustomShaderCase init");
        auto &render = zidian::Instance->getRender();
        
    }

    virtual void onTick(float deltaTime) override{
        // zidian::Log::purple("game", "tick deltaTime = %f", deltaTime);
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
    }

    
    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
    }
};

void TestCustomShader(){
    zidian::AppConfig config;
    config.name = "CustomShader";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCustomShaderCase>());
    app.init(config);
    app.execute();
}


