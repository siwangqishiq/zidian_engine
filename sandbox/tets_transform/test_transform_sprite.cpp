#include "zidian.h"

class TestCanvasTransformSprite : public zidian::IGame {
public:
    std::shared_ptr<zidian::Image> image1;

    std::shared_ptr<zidian::Image> splashImage;

    float angleDegree = 0.0f;

    std::shared_ptr<zidian::Image> spriteImage;
    int spriteIndex = 0;
    float duratingSeconds = 0.8f;
    int frameCount = 6;
    float frameWaitTime = 0.0f;
    float scaleFactor = 1.0f;
    float totalTime = 0.0f;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestCanvasTransformSprite init");
        auto &render = zidian::Instance->getRender();
        splashImage = render->textureManager->loadImageByPath("images/gta6.jpg"); 
        image1 = render->textureManager->loadImageByPath("images/test1.jpg");

        spriteImage = render->textureManager->loadImageByPath("images/player_full_animation.png");
    }

    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        zidian::Rect splashSrcRect = splashImage->getImageRect();
        zidian::Rect splashDstRect{0.0f, 0.0f, static_cast<float>(canvas->getWidth()),  static_cast<float>(canvas->getHeight())};
        canvas->drawImage(splashImage, splashSrcRect, splashDstRect);

        zidian::Rect srcRect = image1->getImageRect();
        zidian::Rect dstRect{0.0f, 0.0f, srcRect.width / 2.0f, srcRect.height / 2.0f};
        canvas->drawImage(image1, srcRect, dstRect);

        canvas->save();
        canvas->translate(200.0f, 0.0f);
        canvas->drawImage(image1, srcRect, dstRect);
        canvas->restore();

        canvas->save();
        canvas->translate(600.0f, 0.0f);
        auto center = dstRect.center();
        canvas->rotate(45.0f, center[0], center[1]);
        canvas->drawImage(image1, srcRect, dstRect);
        canvas->restore();

        // canvas->save();
        // canvas->scale(0.1f, 0.1f);
        // auto subImage = image1->buildSubImage(300.0f,300.0f,100.0f,100.0f);
        // canvas->drawImage(subImage, subImage->getImageRect(), dstRect);
        // canvas->restore();

        float spriteWidth = 40.0f;
        float spriteHeight = 64.0f;

        frameWaitTime += deltaTime;
        if(frameWaitTime >= (duratingSeconds / frameCount)){
            spriteIndex = (spriteIndex + 1) % frameCount;
            frameWaitTime = 0.0f;
        }

        zidian::Rect srcRect2{spriteIndex * spriteWidth, 0, spriteWidth, spriteHeight};
        zidian::Rect dstRect2{0.0f,0.0f,2.0f * spriteWidth,2.0f * spriteHeight};
        canvas->save();
        totalTime += deltaTime;
        scaleFactor = 3.0f * glm::sin(0.5f * totalTime) + 3.0f;
        canvas->scale(scaleFactor,scaleFactor);
        canvas->drawImage(spriteImage, srcRect2, dstRect2);
        canvas->restore();
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
        splashImage = nullptr;
        image1 = nullptr;
        spriteImage = nullptr;
        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestCanvasTransformSpriteCase(){
    zidian::AppConfig config;
    config.name = "TestCanvasTransformSprite";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.isRecordLog = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestCanvasTransformSprite>());
    app.init(config);
    app.execute();
}


