#pragma once

#include <string>
#include <cstdint>

class GLFWwindow;

namespace zidian{
    struct AppConfig{
        std::string name;
        uint32_t windowWidth;
        uint32_t windowHeight;
        bool vsync = true;
    };

    class Application{
    public:
        static std::string TAG;

        Application();

        void init(AppConfig config);

        int execute();

        ~Application();
    private:
        AppConfig config;
        GLFWwindow *mWindow;

        void initWindow();
    };
}