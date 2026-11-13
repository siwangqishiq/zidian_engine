#include "zidian.h"

class TestComposeImage : public zidian::IGame {
public:
    std::shared_ptr<zidian::ComposeImage> composeImage;
    std::shared_ptr<zidian::Image> subImg1;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestComposeImage init");
        auto &render = zidian::Instance->getRender();
        composeImage = render->textureManager->createComposeImage(300, 300, glm::vec4(0.0f, 1.0f, 0.0f , 1.0f));
        // zidian::Log::brown("game", "wh %u %u", composeImage->width, composeImage->height);
        
        uint32_t subWidth = 100;
        uint32_t subHeight = 100;
        const unsigned size = 4 * subWidth * subHeight;
        uint8_t *pixel = new uint8_t[size];
        subImg1 = composeImage->loadImageData(150,150,subWidth, subHeight , size , pixel);
        delete[] pixel;
    }

    virtual void onTick(float deltaTime) override{
        testEmptyImage(deltaTime);
    }

    void testEmptyImage(float deltaTime){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        zidian::Rect srcRect = composeImage->getImageRect();
        zidian::Rect dstRect = srcRect;

        // zidian::Log::brown("game", "%f %f", srcRect.width, srcRect.height);
        canvas->drawImage(composeImage, srcRect, dstRect);
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestComposeImage dispose");

        composeImage = nullptr;
        subImg1 = nullptr;
        
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestComposeImageCase(){
    zidian::AppConfig config;
    config.name = "TestSpriteDraw";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestComposeImage>());
    app.init(config);
    app.execute();
}


