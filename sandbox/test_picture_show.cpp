#include "zidian.h"
#include <vector>

class TestPictureShowGame : public zidian::IGame {
public:
    int picWidth;
    int picHeight;
    int picChannel;
    // std::vector<std::vector<glm::vec4>> pixels;
    glm::vec4 *pixels = nullptr;

    virtual void onInit() override{
        zidian::Log::brown("game", "TestPictureShow init");
        auto &render = zidian::Instance->getRender();
        zidian::RandomUtil::setRandomSeed(0);

        uint8_t* rawPixles = zidian::AssetManager::getInstance()->readAssetImageFile("images/test1.jpg", picWidth, picHeight, picChannel);
        zidian::Log::brown("game", "image info : %d %d %d", picWidth , picHeight , picChannel);

        if(rawPixles != nullptr){
            // pixels.resize(picHeight, std::vector<glm::vec4>(picWidth , glm::vec4{}));
            pixels = new glm::vec4[picWidth * picHeight];
            int pixelOffset = 0;
            
            for(int i = 0 ; i < picHeight; i++) {
                for(int j = 0; j < picWidth; j++){
                    // pixels[picWidth * i + j] = glm::vec4(0.0f, 1.0f, 0.0f , 1.0f);
                    // pixels[picWidth * i + j] = glm::vec4(0.612f, 0.788f, 0.910f, 1.0f);
                    
                    float r = static_cast<float>(rawPixles[pixelOffset + 0]) / 255.0f;
                    float g = static_cast<float>(rawPixles[pixelOffset + 1]) / 255.0f;
                    float b = static_cast<float>(rawPixles[pixelOffset + 2]) / 255.0f;
                    float a = static_cast<float>(rawPixles[pixelOffset + 3]) / 255.0f;

                    // pixels[picWidth * i + j] = glm::vec4(r,g,b,a);

                    pixels[picWidth * i + j] = glm::vec4(
                        zidian::Utils::SrgbToLinear(r),
                        zidian::Utils::SrgbToLinear(g),
                        zidian::Utils::SrgbToLinear(b), 
                        zidian::Utils::SrgbToLinear(a)
                    );

                    pixelOffset += 4;
                }//end for j
            }

            zidian::AssetManager::getInstance()->freePixels(rawPixles);
        }
    }

    virtual void onTick() override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();

        for(int i = 0 ; i< picHeight; i++) {
            for(int j = 0; j< picWidth; j++){
                canvas->drawSimpleRect(j, i, 1, 1, pixels[picWidth * i + j]);
                // glm::vec2 c(static_cast<float>(j), static_cast<float>(i));
                // canvas->drawSimpleCircle(c , 1.0f, pixels[picWidth * i + j]);
            }//end for j
        }
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "TestMultiPipeline dispose");
        delete[] pixels;
        pixels = nullptr;

        zidian::AudioManager::getInstance()->dispose();
    }
};

void TestPictureShow(){
    zidian::AppConfig config;
    config.name = "TestPictureShow";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.isRecordLog = true;
    config.vsync = true;
    config.isFullScreen = false;
    config.clearColor = glm::vec4(0.0f, 0.0f , 0.0f , 1.0f);

    zidian::Application app;
    app.setGameObject(std::make_shared<TestPictureShowGame>());
    app.init(config);
    app.execute();
}


