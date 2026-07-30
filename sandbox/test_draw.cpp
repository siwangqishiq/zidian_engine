#include "zidian.h"

class TestDrawApp : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "GameApp TestDraw init");
        auto &render = zidian::Instance->getRender();
        zidian::Log::brown("game", "canvas size %d x %d", 
                render->getCanvas()->getWidth(), 
                render->getCanvas()->getHeight());
    }
    
    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        canvas->drawRect(0.0f, 0.0f, canvas->getWidth()/2, canvas->getHeight()/2, {1.0f, 0.0f , 0.0f , 1.0f});
        canvas->drawRect(canvas->getWidth()/2, 0.0f, canvas->getWidth()/2, canvas->getHeight()/2, {0.0f, 1.0f , 0.0f , 1.0f});
        canvas->drawRect(0.0f, canvas->getHeight()/2, canvas->getWidth()/2, canvas->getHeight()/2, {0.0f, 0.0f , 1.0f , 1.0f});
        canvas->drawRect(canvas->getWidth()/2, canvas->getHeight()/2, canvas->getWidth()/2, canvas->getHeight()/2, {1.0f, 1.0f , 0.0f , 1.0f});
    }

    void test2(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        canvas->drawRect(0.0f, 0.0f, canvas->getWidth(), canvas->getHeight(), {0.0f, 1.0f , 0.0f , 1.0f});
    }

    void drawRect(){
        auto &render = zidian::Instance->getRender();
        const uint8_t vertexCount = 3; 
        glm::vec2 vertices[vertexCount] = {
            {-0.5f,   0.5f},
            { 0.5f ,  0.5f},
            { 0.0f , -0.5f}
        };
        glm::vec4 color = {1.0f, 0.0f , 0.0f, 1.0f};

        render->getCanvas()->drawTriangle(vertices, color);
        render->getCanvas()->drawRect(0.0f, 0.0f, 0.8f, 0.8f, {0.0f, 1.0f , 0.0f , 1.0f});
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
    }
};


void TestDraw(){
    zidian::AppConfig config;
    config.name = "Test Draw Canvas";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
    config.vsync = true;

    std::string content = zidian::AssetManager::getInstance()->readAssetFileAsString("test.txt");
    zidian::Log::green("test", "text:%s",content.c_str());

    zidian::Application app;
    app.setGameObject(std::make_shared<TestDrawApp>());
    app.init(config);
    app.execute();
}




