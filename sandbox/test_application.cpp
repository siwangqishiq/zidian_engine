#include "zidian.h"

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
    app.init(config);
    app.execute();
}


