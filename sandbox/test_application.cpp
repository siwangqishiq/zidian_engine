#include "zidian.h"

class GameApp : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "game init");
        zidian::Instance->getTaskSchedule()->scheduleAtFixedRate([](void *){
            zidian::Log::brown("game", "game tick");
        },1000);
    }
    
    virtual void onTick() override{
        // zidian::Log::brown("game", "game tick");
        auto &render = zidian::Instance->getRender();
        
        const uint8_t vertexCount = 3; 
        glm::vec2 vertices[vertexCount] = {
            {-0.5f,   0.5f},
            { 0.5f ,  0.5f},
            { 0.0f , -0.5f}
        };
        glm::vec4 colors[vertexCount] = {
            {1.0f, 0.0f , 0.0f, 1.0f},
            {0.0f, 1.0f , 0.0f, 1.0f},
            {0.0f, 0.0f , 1.0f, 1.0f}
        };
        render->getCanvas()->drawTriangles(vertices, colors, vertexCount);
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
    }
};


void TestApplication(){
    zidian::AppConfig config;
    config.name = "ZidianApp";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
    config.vsync = true;

    std::string content = zidian::AssetManager::getInstance()->readAssetFileAsString("test.txt");
    zidian::Log::green("test", "text:%s",content.c_str());

    zidian::Application app;
    app.setGameObject(std::make_shared<GameApp>());
    app.init(config);
    app.execute();
}




