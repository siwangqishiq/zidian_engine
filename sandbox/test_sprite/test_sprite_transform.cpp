#include "zidian.h"

class TestSpriteTransform : public zidian::IGame {
public:
    std::shared_ptr<zidian::Image> image1;

    std::shared_ptr<zidian::Image> splashImage;

    float angleDegree = 0.0f;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestSpriteTransform init");
        auto &render = zidian::Instance->getRender();
        splashImage = render->textureManager->loadImageByPath("images/gta6.jpg"); 
        image1 = render->textureManager->loadImageByPath("images/test1.jpg");
    }

    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        zidian::Rect splashSrcRect = splashImage->getImageRect();
        zidian::Rect splashDstRect{0.0f, 0.0f, static_cast<float>(canvas->getWidth()),  static_cast<float>(canvas->getHeight())};
        canvas->drawImage(splashImage, splashSrcRect, splashDstRect);

        zidian::Rect srcRect = image1->getImageRect();
        zidian::Rect dstRect{0.0f, 0.0f, srcRect.width / 2.0f, srcRect.height / 2.0f};
        canvas->drawImage(image1, srcRect, dstRect, {1.0f, 1.0f , 1.0f , 0.5f}, dstRect.center(), angleDegree);
        angleDegree += 0.2f;
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
        splashImage = nullptr;
        image1 = nullptr;
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestSpriteTransformCase(){
    zidian::AppConfig config;
    config.name = "TestSpriteTransform";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestSpriteTransform>());
    app.init(config);
    app.execute();
}


