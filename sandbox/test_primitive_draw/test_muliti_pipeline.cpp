#include "zidian.h"

class TestMultiPipeline : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "TestMultiPipeline init");
        auto &render = zidian::Instance->getRender();
        zidian::RandomUtil::setRandomSeed(0);
    }

    virtual void onTick(float deltaTime) override{
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

        x += size;
        float radius = size / 2.0f;
        canvas->drawSimpleCircle(glm::vec2(x +  radius, radius) , radius, glm::vec4(1.0f, 0.0f , 0.0f, 1.0f));

        x += size;
        canvas->drawSimpleCircle(glm::vec2(x + radius, radius) , radius, glm::vec4(0.0f, 1.0f , 0.0f, 1.0f));

        x += size;
        canvas->drawSimpleCircle(glm::vec2(x + radius, radius) , radius, glm::vec4(0.0f, 0.0f , 1.0f, 1.0f));

        x += size;
        canvas->drawSimpleRect(x, 0.0f, size, size, glm::vec4(0.0f, 1.0f , 0.0f, 1.0f));

        x += size;
        canvas->drawSimpleCircle(glm::vec2(x + radius, radius) , radius, glm::vec4(0.0f, 1.0f , 1.0f, 1.0f));

        float y = size;

        canvas->drawSimpleTriangle(glm::vec2(size + size / 2.0f, y), glm::vec2(size, y + size), glm::vec2(size + size, y + size) , glm::vec4(1.0f, 1.0f , 0.0f, 1.0f));

        canvas->drawSimpleTriangle(glm::vec2(150.0f, 200.0f), 
            glm::vec2(150.0f + 0.0f, 200.0f + 100.0f), 
            glm::vec2(150.0f + 150.0f, 200.0f + 100.0f), 
            glm::vec4(0.0f, 0.0f , 1.0f, 1.0f));
        
        drawOneCircle();
        // canvas->drawRect(0.0f, 0.0f, canvas->getWidth()/2, canvas->getHeight()/2, {1.0f, 0.0f , 0.0f , 1.0f});
        // canvas->drawRect(canvas->getWidth()/2, 0.0f, canvas->getWidth()/2, canvas->getHeight()/2, {0.0f, 1.0f , 0.0f , 1.0f});
        // canvas->drawRect(0.0f, canvas->getHeight()/2, canvas->getWidth()/2, canvas->getHeight()/2, {0.0f, 0.0f , 1.0f , 1.0f});
        // canvas->drawRect(canvas->getWidth()/2, canvas->getHeight()/2, canvas->getWidth()/2, canvas->getHeight()/2, {1.0f, 1.0f , 0.0f , 1.0f});
    }

    void drawOneCircle(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float cx = canvas->getWidth() / 2.0f;
        float cy = canvas->getHeight() / 2.0f;
        float radius = canvas->getHeight() / 4.0f;
        glm::vec4 color = {1.0f, 1.0f , 0.0f, 1.0f};
        canvas->drawCircle(cx,cy, radius, color);
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
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestMultiPipeline>());
    app.init(config);
    app.execute();
}


