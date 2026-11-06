#include "zidian.h"

class TestCustomShaderCase : public zidian::IGame {
public:
    zidian::CustomShaderPipelineId pid1 = -1;
    zidian::CustomShaderPipelineId pid2 = -1;

    zidian::CustomShaderPipelineId gridShader = -1;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestCustomShaderCase init");
        auto &render = zidian::Instance->getRender();
        
        pid1 = render->pipelineManager->createCustomShaderPipeline("custom_shader1");
        pid2 = render->pipelineManager->createCustomShaderPipeline("custom_shader2");

        gridShader = render->pipelineManager->createCustomShaderPipeline("custom_grid");

        zidian::Instance->getThreadPool()->enqueue([](){
            zidian::AudioManager::getInstance()->loadAudio("sound/lace.mp3","lace",true);
            zidian::AudioManager::getInstance()->playAudio("lace");
        });
    }

    virtual void onTick(float deltaTime) override{
        // zidian::Log::purple("game", "tick deltaTime = %f", deltaTime);
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        // canvas->drawCustomShader(pid1, 0.0f, 0.0f, 300.0f , 300.0f);
        canvas->drawCustomShader(pid2, 600.0f, 0.0f, 200.0f , 200.0f);
        canvas->drawCustomShader(gridShader , 0.0f, 0.0f, 600.0f , 600.0f);
    }
    
    virtual void onDispose() override {
        zidian::AudioManager::getInstance()->dispose();
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
    config.clearColor = glm::vec4(1.0f, 1.0f , 1.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCustomShaderCase>());
    app.init(config);
    app.execute();
}


