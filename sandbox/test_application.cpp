#include "zidian.h"

void TestApplication(){
    zidian::AppConfig config;
    config.name = "sandBox";
    config.windowWidth = 1280;
    config.windowHeight = 800;

    zidian::Application app;
    app.init(config);
    app.execute();
}


