#include "application.h"
#include "utils/log.h"
#include "const.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "renderer/render.h"

namespace zidian{
    std::string Application::TAG = "app_tag";

    Application::Application(){
        Log::i(TAG,"Application construct");
    }

    void Application::init(AppConfig cfg){
        Log::i(TAG,"Application init");
        config = cfg;

        initWindow();

        render = std::make_unique<Render>();
        render->initVulkan();
    }

    void Application::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(config.windowWidth, config.windowHeight, 
            config.name.c_str(), nullptr, nullptr);
        glfwSetWindowPos(window, 32, 64);
        glfwSwapInterval(config.vsync?1:0);//启动垂直同步
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            if(key == GLFW_KEY_ESCAPE){
                glfwSetWindowShouldClose(window, true);
            }   
        });
    }

    int Application::execute(){
        Log::i(TAG,"Application execute");
        while(!glfwWindowShouldClose(window)){
            glfwPollEvents();
            // todo draw frame use vulkan
        }//end while

        onDispose();
        glfwDestroyWindow(window);
        return SUCCESS;
    }

    void Application::onDispose() {
        Log::i(TAG,"Application onDispose");
        render->onDispose();
    }

    Application::~Application(){
        Log::i(TAG,"Application deconstructed");
    }
}