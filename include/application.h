#pragma once

#include <string>
#include <cstdint>
#include <memory>

class GLFWwindow;

namespace zidian{
    struct AppConfig{
        std::string name;
        uint32_t windowWidth;
        uint32_t windowHeight;
        bool vsync = true;
    };

    class Render;

    class Application{
    public:
        static std::string TAG;

        Application();

        void init(AppConfig config);

        int execute();

        void onDispose();

        ~Application();

    protected:
        std::unique_ptr<Render> render;

    private:
        AppConfig config;
        GLFWwindow *window;

        void initWindow();
    };
}