#include "application.h"
#include "utils/log.h"
#include "const.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace zidian{
    std::string Application::TAG = "app_tag";

    Application::Application(){
        Log::i(TAG,"Application construct");
    }

    void Application::init(AppConfig cfg){
        Log::i(TAG,"Application init");
        config = cfg;

        initWindow();
    }

    void Application::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        mWindow = glfwCreateWindow(config.windowWidth, config.windowHeight, 
            config.name.c_str(), nullptr, nullptr);
        glfwSetWindowPos(mWindow, 32, 64);
        glfwSwapInterval(config.vsync?1:0);//启动垂直同步
        glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            if(key == GLFW_KEY_ESCAPE){
                glfwSetWindowShouldClose(window, true);
            }   
        });
    }

    int Application::execute(){
        Log::i(TAG,"Application execute");

        while(!glfwWindowShouldClose(mWindow)){
            glfwPollEvents();
            // todo draw frame use vulkan
        }//end while
        glfwDestroyWindow(mWindow);
        return SUCCESS;
    }

    Application::~Application(){
        Log::i(TAG,"Application deconstructed");
    }
}