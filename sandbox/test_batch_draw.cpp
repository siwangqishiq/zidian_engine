#include "zidian.h"

class BatchDraw : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("game", "GameApp TestDraw init");
        auto &render = zidian::Instance->getRender();
        zidian::Log::brown("game", "canvas size %d x %d", 
                render->getCanvas()->getWidth(), 
                render->getCanvas()->getHeight());
        zidian::RandomUtil::setRandomSeed(0);
    }
    
    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        float width = 5.0f;
        float height = 5.0f;
        int vCount = 0;
        for(int i = 0; i < canvas->getHeight(); i+= width){
            for(int j = 0; j < canvas->getWidth(); j+= height){
                glm::vec4 color = {
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),  1.0f};
                canvas->drawRect(j, i, width, height, color);
                vCount++;
            }//
        }
        
        // std::cout << "vCount :" << vCount << std::endl;
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
    }
};


void TestBatchDraw(){
    zidian::AppConfig config;
    config.name = "Test BatchDraw Canvas";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    config.vsync = true;

    std::string content = zidian::AssetManager::getInstance()->readAssetFileAsString("test.txt");
    zidian::Log::green("test", "text:%s",content.c_str());

    zidian::Application app;
    app.setGameObject(std::make_shared<BatchDraw>());
    app.init(config);
    app.execute();
}




