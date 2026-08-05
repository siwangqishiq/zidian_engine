#include "zidian.h"

class TestPrimitiveDraw : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "GameApp init");
        auto &render = zidian::Instance->getRender();
        zidian::RandomUtil::setRandomSeed(0);

        // zidian::AudioManager::getInstance()->loadAudio("sound/ci.mp3","bgm");
        // zidian::AudioManager::getInstance()->playAudio("bgm");
    }

    void drawOneCircle(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float cx = canvas->getWidth() / 2.0f;
        float cy = canvas->getHeight() / 2.0f;
        float radius = canvas->getHeight() / 4.0f;
        glm::vec4 color = {0.0f, 0.0f , 1.0f, 1.0f};
        canvas->drawCircle(cx,cy, radius, color);
    }
    
    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        const float radius = 40.0f;
        for(int i = 0; i < canvas->getHeight(); i+= 2.0f * radius){
            for(int j = 0; j < canvas->getWidth(); j+= 2.0f * radius){
                glm::vec4 color = {
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),  1.0f};
                canvas->drawCircle(j + radius, i + radius, radius, color);
            }//
        }
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
        zidian::AudioManager::getInstance()->dispose();
    }
};


void TestCircleDraw(){
    zidian::AppConfig config;
    config.name = "Test BatchDraw Canvas";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    config.vsync = true;
    config.isFullScreen = false;

    zidian::Application app;
    app.setGameObject(std::make_shared<TestPrimitiveDraw>());
    app.init(config);
    app.execute();
}




