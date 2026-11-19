#include "zidian.h"

class TestComposeImage : public zidian::IGame {
public:
    std::shared_ptr<zidian::ComposeImage> composeImage;
    std::shared_ptr<zidian::Image> subImg1;
    std::shared_ptr<zidian::Image> subImg2;
    std::shared_ptr<zidian::Image> subImg3;
    std::shared_ptr<zidian::Image> spriteImage;

    int spriteIndex = 0;
    float duratingSeconds = 0.8f;
    int frameCount = 6;
    float frameWaitTime = 0.0f;


    std::vector<std::shared_ptr<zidian::Image>> subImages;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestComposeImage init");
        // testEmptyImageInit();
        // testCase2Init(); 
        testCase3Init();
    }

    virtual void onTick(float deltaTime) override{
        // testEmptyImage(deltaTime);
        testCase3();
        testSprite(deltaTime);
    }

    void testCase3Init(){
        auto &render = zidian::Instance->getRender();
        composeImage = render->textureManager->createComposeImage(
            2048, 2048, 
            glm::vec4(1.0f, 1.0f, 1.0f , 1.0f)
        );

        subImg1 = composeImage->autoLoadImageFromAssetImageFile("images/test2.jpg");
        subImg2 = composeImage->autoLoadImageFromAssetImageFile("images/test3.png");

        const int size = 20;
        subImages.resize(size);
        for(int i = 0 ; i < size; i++){
            subImages[i] = composeImage->autoLoadImageFromAssetImageFile("images/test3.png");
        }

        spriteImage = composeImage->autoLoadImageFromAssetImageFile("images/player_full_animation.png");
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

        zidian::Rect srcRect{spriteIndex * spriteWidth, 0, spriteWidth, spriteHeight};
        zidian::Rect dstRect{0.0f,0.0f,2.0f * spriteWidth,2.0f * spriteHeight};
        canvas->drawImage(spriteImage, srcRect, dstRect);
    }

    void testCase2Init(){
        auto &render = zidian::Instance->getRender();
        composeImage = render->textureManager->createComposeImage(
            2048, 2048, 
            glm::vec4(1.0f, 1.0f, 1.0f , 1.0f)
        );

        if(composeImage == nullptr){
            zidian::Log::e("game", "alloc composeImage error");
            return;
        }

        int imageWidth;
        int imageHeight;
        int channel;
        // auto pixels = zidian::AssetManager::getInstance()->readAssetImageFile("images/test3.png",imageWidth, imageHeight,channel);
        auto pixels = zidian::AssetManager::getInstance()->readAssetImageFile("images/test2.jpg",imageWidth, imageHeight,channel);
        if(pixels){
            subImg1 = composeImage->loadImageData(0,0,imageWidth, imageHeight, pixels);
            zidian::AssetManager::getInstance()->freePixels(pixels);
        }

        int imageWidth2;
        int imageHeight2;
        pixels = zidian::AssetManager::getInstance()->readAssetImageFile("images/test3.png",imageWidth2, imageHeight2,channel);
        if(pixels){
            subImg2 = composeImage->loadImageData(imageWidth,0,imageWidth2, imageHeight2, pixels);
            zidian::AssetManager::getInstance()->freePixels(pixels);
        }
    }

    void testCase3(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        zidian::Rect srcRect = composeImage->getImageRect();
        const float srcW = (srcRect.width / srcRect.height) * canvas->getHeight();
        zidian::Rect dstRect {0.0f, 0.0f, srcW, static_cast<float>(canvas->getHeight())};
        canvas->drawImage(composeImage, srcRect, dstRect);
    }

    void testCase2(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        // zidian::Rect srcRect = composeImage->getImageRect();
        // const float srcW = (srcRect.width / srcRect.height) * canvas->getHeight();
        // zidian::Rect dstRect {0.0f, 0.0f, srcW, static_cast<float>(canvas->getHeight())};
        // canvas->drawImage(composeImage, srcRect, dstRect);

        // zidian::Rect srcRect = subImg1->getImageRect();
        // const float srcW = (srcRect.width / srcRect.height) * canvas->getHeight();

        // // srcRect.left = srcRect.width / 2.0f;
        // // srcRect.top = srcRect.height / 2.0f;
        // // srcRect.width = srcRect.width / 2.0f;
        // // srcRect.height = srcRect.height / 2.0f;

        // zidian::Rect dstRect {0.0f, 0.0f, srcW, static_cast<float>(canvas->getHeight())};
        // // zidian::Log::brown("game", "composeImage pos %f %f", dstRect.width, dstRect.height);
        // canvas->drawImage(subImg1, srcRect, dstRect);

        zidian::Rect srcRect = subImg2->getImageRect();
        const float srcW = (srcRect.width / srcRect.height) * canvas->getHeight();
        zidian::Rect dstRect {0.0f, 0.0f, srcW, static_cast<float>(canvas->getHeight())};
        canvas->drawImage(subImg2, srcRect, dstRect);
    }

    void testEmptyImageInit(){
        auto &render = zidian::Instance->getRender();
        composeImage = render->textureManager->createComposeImage(1024, 1024, 
            glm::vec4(1.0f, 1.0f, 1.0f , 1.0f));
        // zidian::Log::brown("game", "wh %u %u", composeImage->width, composeImage->height);
        
        uint32_t subWidth = 100;
        uint32_t subHeight = 100;
        const unsigned size = 4 * subWidth * subHeight;
        uint8_t *pixel = new uint8_t[size];
        int index = 0;
        for(int i = 0 ; i < subHeight ;i++){
            for(int j = 0 ; j < subWidth; j++){
                pixel[index++] = 0;
                pixel[index++] = 0;
                pixel[index++] = 255;
                pixel[index++] = 255; 
            }
        }
        subImg1 = composeImage->loadImageData(100,20,subWidth, subHeight, pixel);
        delete[] pixel;
    }

    void testEmptyImage(float deltaTime){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        zidian::Rect srcRect = composeImage->getImageRect();
        zidian::Rect dstRect = srcRect;

        // zidian::Log::brown("game", "%f %f", srcRect.width, srcRect.height);
        // canvas->drawImage(composeImage, srcRect, dstRect);

        zidian::Rect srcRect1 = subImg1->getImageRect();
        zidian::Rect dstRect1 = srcRect1;
        dstRect1.left = 300.0f + 2.0f;
        canvas->drawImage(subImg1, srcRect1, dstRect1);
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestComposeImage dispose");

        composeImage = nullptr;
        subImg1 = nullptr;
        subImg2 = nullptr;
        subImg3 = nullptr;
        subImages.clear();
        spriteImage = nullptr;

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


