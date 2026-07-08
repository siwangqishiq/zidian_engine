#include "application.h"
#include "utils/log.h"
#include "const.h"

namespace zidian{
    std::string Application::TAG = "app_tag";

    Application::Application(){
        Log::i(TAG,"Application construct");
    }

    void Application::init(AppConfig config){
        Log::i(TAG,"Application init");
        this->config = config;
    }

    int Application::execute(){
        Log::i(TAG,"Application execute");
        return SUCCESS;
    }

    Application::~Application(){
        Log::i(TAG,"Application deconstructed");
    }
}