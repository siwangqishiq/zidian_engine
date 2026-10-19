#include "zidian.h"

class TestSpriteDraw : public zidian::IGame {
public:
    std::shared_ptr<zidian::Image> image1;
    std::shared_ptr<zidian::Image> image2;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestSprite init");
        auto &render = zidian::Instance->getRender();
        
        image1 = render->textureManager->loadImageByPath("images/test1.jpg");
        image2 = render->textureManager->loadImageByPath("images/test2.jpg");
    }

    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        canvas->drawCircle(canvas->getWidth() / 2.0f, canvas->getHeight() / 2.0f, 200.0f, glm::vec4(0.0f, 1.0f , 0.0f, 1.0f));
        
        zidian::Rect rect = image1->getImageRect();
        float dstWidth = canvas->getWidth() / 2.0f;
        float dstHeight = dstWidth / (static_cast<float>(image1->width) / static_cast<float>(image1->height));
        zidian::Rect dstRect{
            .left = 0.0f,
            .top = 0.0f,
            .width = dstWidth,
            .height = dstHeight
        };
        canvas->drawImage(image1, rect, dstRect);
        
        zidian::Rect srcRect2 = image2->getImageRect();
        // srcRect2.width = srcRect2.width / 2.0f;
        // srcRect2.height = srcRect2.height / 2.0f;

        float dstHeight2 = dstHeight;
        float dstWidth2 = (static_cast<float>(image2->width) / static_cast<float>(image2->height)) * dstHeight2;
        zidian::Rect dstRect2{
            .left = canvas->getWidth() / 2.0f,
            .top = 0.0f,
            .width = dstWidth2,
            .height = dstHeight2
        };
        canvas->drawImage(image2, srcRect2, dstRect2);
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
        image1 = nullptr;
        image2 = nullptr;
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestSpriteDraw1(){
    zidian::AppConfig config;
    config.name = "TestSpriteDraw";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestSpriteDraw>());
    app.init(config);
    app.execute();
}


