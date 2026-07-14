#include "application.h"
#include "utils/log.h"
#include "const.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "widget/task_schedule.h"
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
        findGlfwExtension();

        taskSchedule = std::make_unique<TaskSchedule>();
        taskSchedule->scheduleAtFixedRate([this](void *){
            Log::w("fps", "current fps : %d", fps);
            fps = 0;
        },1000);

        render = std::make_unique<Render>(*this);

        auto glfwExtension = findGlfwExtension();
        render->initVulkan(glfwExtension);
    }

    std::vector<const char*> Application::findGlfwExtension(){
        uint32_t glfwExtensionCount = 0;
        const char** extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensionList;
        for(uint32_t i=0; i < glfwExtensionCount; i++){
            extensionList.push_back(extensions[i]);
        }
        return extensionList;
    }

    void Application::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(config.windowWidth, config.windowHeight, 
            config.name.c_str(), nullptr, nullptr);
        glfwSetWindowPos(window, 32, 64);
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            if(key == GLFW_KEY_ESCAPE){
                glfwSetWindowShouldClose(window, true);
            }   
        });

        // 设置opengl上下文
        // glfwMakeContextCurrent(window);
        // glfwSwapInterval(config.vsync?1:0);//启动垂直同步
    }

    std::unique_ptr<Render>& Application::getRender(){
        return this->render;
    }

    AppConfig& Application::getAppConfig() {
        return config; 
    }

    VkSurfaceKHR Application::createSurfaceFromInstance(VkInstance instance){
        VkSurfaceKHR surface;
        auto ret = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if(ret != VK_SUCCESS){
            Log::e(TAG, "create surface error %d",ret);
            return VK_NULL_HANDLE;            
        }
        return surface;
    }

    void Application::getFramebufferSize(int &width, int &height){
        glfwGetFramebufferSize(window, &width, &height);
    }

    int Application::execute(){
        Log::i(TAG,"Application execute");
        while(!glfwWindowShouldClose(window)){
            glfwPollEvents();
            tick();
            // glfwSwapBuffers(window);
        }//end while

        onDispose();
        glfwDestroyWindow(window);
        return SUCCESS;
    }

    void Application::tick(){
        getRender()->beginRenderFrame();

        taskSchedule->tick();
        getRender()->endRenderFrame();

        fps++;
    }

    void Application::onDispose() {
        Log::i(TAG,"Application onDispose");
        render->onDispose();
    }

    Application::~Application(){
        Log::i(TAG,"Application deconstructed");
    }
}