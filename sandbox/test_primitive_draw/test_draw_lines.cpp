#include "zidian.h"

class TestDrawLines : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("TestDrawLines", "GameApp TestDraw init");
    }
    
    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float screenWidth = static_cast<float>(canvas->getWidth());
        float screenHeight = static_cast<float>(canvas->getHeight());
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
    }

    void drawLinesAndCircle() {
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float screenWidth = static_cast<float>(canvas->getWidth());
        float screenHeight = static_cast<float>(canvas->getHeight());

        const float PI2 = 2.0f * 3.1415926f;
        float A = screenHeight / 2.0f;
        std::vector<glm::vec2> points;
        for(float step = 0.0f; step < screenWidth ; step += 1.0f){
            float x = step;
            float y = A*cos((PI2 / screenWidth) * step) + A + 50.0f;
            points.push_back({x,y});
        }
        canvas->drawLines(points.data(), points.size(), 5.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        canvas->drawCircle(screenWidth / 2.0f, screenHeight/2.0f, 100.0f, {1.0f, 0.0f, 0.0f, 1.0f});
    }

    void drawLine(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float screenWidth = static_cast<float>(canvas->getWidth());
        float screenHeight = static_cast<float>(canvas->getHeight());
        canvas->drawLine({0.0f, 0.0f}, {screenWidth, screenHeight}, 10.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
        canvas->drawLine({screenWidth, 0.0f}, {0.0f, screenHeight}, 20.0f, glm::vec4(1.0f, 1.0f, 0.0f, 0.5f));
    }
};


void TestDrawLinesCase(){
    zidian::AppConfig config;
    config.name = "Test Draw Lines";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    config.vsync = true;
    config.isRecordLog = true;

    zidian::Application app;
    app.setGameObject(std::make_shared<TestDrawLines>());
    app.init(config);
    app.execute();
}




