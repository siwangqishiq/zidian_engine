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

        AppConfig& getAppConfig();
        
        GLFWwindow *window;

        VkSurfaceKHR createSurfaceFromInstance(VkInstance instance);

        void getFramebufferSize(int &width, int &height);
    protected:
        std::unique_ptr<Render> render;
        std::unique_ptr<TaskSchedule> taskSchedule;
        AppConfig config;
    private:
        void initWindow();
        void tick();

        int fps = 0;
        std::vector<const char*> findGlfwExtension();
    };
}