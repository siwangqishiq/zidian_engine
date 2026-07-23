#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include "vulkan/vulkan.h"
#include "glm/glm.hpp"

class GLFWwindow;

namespace zidian{
    struct AppConfig{
        std::string name;
        uint32_t windowWidth;
        uint32_t windowHeight;
        bool vsync = true;
        glm::vec4 clearColor = {0.1f, 0.1f , 0.1f, 1.0f};
    };

    class IGame{
    public:
        virtual void onInit() = 0;
        virtual void onTick() = 0;
        virtual void onDispose() = 0;
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

        void onDrawFrame();

        ~Application();

        std::unique_ptr<Render>& getRender();
        std::unique_ptr<TaskSchedule>& getTaskSchedule();
        AppConfig& getAppConfig();
        
        GLFWwindow *window;

        VkSurfaceKHR createSurfaceFromInstance(VkInstance instance);

        void getFramebufferSize(int &width, int &height);

        void setGameObject(std::shared_ptr<IGame> obj);
    protected:
        std::unique_ptr<Render> render;
        std::unique_ptr<TaskSchedule> taskSchedule;
        AppConfig config;
        std::shared_ptr<IGame> game = nullptr;
    private:
        void initWindow();
        void tick();

        int fps = 0;
        std::vector<const char*> findGlfwExtension();
    };
}