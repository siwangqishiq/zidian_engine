#include "zidian.h"

class TestSpriteDraw : public zidian::IGame {
public:
    std::shared_ptr<zidian::Image> image1;
    std::shared_ptr<zidian::Image> image2;
    std::shared_ptr<zidian::Image> subImage1;

    std::shared_ptr<zidian::Image> spriteImage;
    int spriteIndex = 0;
    float duratingSeconds = 0.8f;
    int frameCount = 6;
    float frameWaitTime = 0.0f;

    std::shared_ptr<zidian::Image> emptyImage;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestSprite init");
        auto &render = zidian::Instance->getRender();
        
        image1 = render->textureManager->loadImageByPath("images/test1.jpg");
        image2 = render->textureManager->loadImageByPath("images/test2.jpg");


        subImage1 = image1->buildSubImage(image1->width/ 2 , image1->height / 2, image1->width/ 2, image1->height / 2);
        image2 = subImage1->buildSubImage(subImage1->width / 2 , subImage1->height / 2, subImage1->width/ 2, subImage1->height / 2);

        spriteImage = render->textureManager->loadImageByPath("images/player_full_animation.png");

        // emptyImage = render->textureManager->createEmptyImage(512, 512, glm::vec4(0.0f, 1.0f, 0.0f , 0.5f));
    }

    virtual void onTick(float deltaTime) override{
        // zidian::Log::purple("game", "tick deltaTime = %f", deltaTime);
        // testEmptyImage(deltaTime);
        testSprite(deltaTime);
        // testSubImage();
        // testBaseSprite();
    }

    void testEmptyImage(float deltaTime){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        zidian::Rect srcRect = emptyImage->getImageRect();
        zidian::Rect dstRect = srcRect;
        canvas->drawImage(emptyImage, srcRect, dstRect);
    }

    void testSprite(float deltaTime){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        float spriteWidth = 40.0f;
        float spriteHeight = 64.0f;

        frameWaitTime += deltaTime;
        if(frameWaitTime >= (duratingSeconds / frameCount)){
            spriteIndex = (spriteIndex + 1) % frameCount;
            frameWaitTime = 0.0f;
        }

        zidian::Rect srcRect{spriteWidth + spriteIndex * spriteWidth, 0, spriteWidth, spriteHeight};
        zidian::Rect dstRect{0.0f,0.0f,2.0f * spriteWidth,2.0f * spriteHeight};
        canvas->drawImage(spriteImage, srcRect, dstRect);
    }

    void testSubImage(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        zidian::Rect srcRect = subImage1->getImageRect();
        zidian::Rect dstRect = srcRect;
        canvas->drawImage(subImage1, srcRect, dstRect);

        zidian::Rect srcRect2 = image2->getImageRect();
        zidian::Rect dstRect2 = srcRect2;
        dstRect2.left = srcRect.right();
        canvas->drawImage(image2, srcRect2, dstRect2);
    }

    void testBaseSprite(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        const float radius = 40.0f;
        for(int i = 0; i < canvas->getHeight(); i+= 2.0f * radius){
            for(int j = 0; j < canvas->getWidth(); j+= 2.0f * radius){
                glm::vec4 color = {
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),
                    zidian::RandomUtil::randomFloat(0.0f, 1.0f),  0.5f};
                canvas->drawCircle(j + radius, i + radius, radius, color);
            }//
        }//end for i

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
        canvas->drawImage(image1, rect, dstRect, glm::vec4(1.0f, 1.0f , 0.0f , 1.0f));
        
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
        subImage1 = nullptr;
        spriteImage = nullptr;
        emptyImage = nullptr;
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


