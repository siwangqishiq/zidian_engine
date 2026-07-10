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
    class TaskSchedule;

    class Application{
    public:
        static std::string TAG;

        Application();

        void init(AppConfig config);

        int execute();

        void onDispose();

        ~Application();

        std::unique_ptr<Render>& getRender();

        AppConfig& getAppConfig();
        
    protected:
        std::unique_ptr<Render> render;
        std::unique_ptr<TaskSchedule> taskSchedule;
        AppConfig config;
    private:
        GLFWwindow *window;

        void initWindow();
        void tick();
        int fps = 0;
    };
}