#include <chrono>
#include <sstream>
#include <iomanip>
#include "utils/log.h"
#include "utils/time.h"

namespace zidian {
    const std::string Log::TAG = "log";
    const std::string Log::TIME_FORMAT = "%H:%M:%S";


    using SystemClk = std::chrono::system_clock;

#ifdef __ANDROID__
    #include <android/log.h>

    #define  LOG_TAG    "zidian"
    #define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, __VA_ARGS__)
    #define  LOGW(...)  __android_log_print(purple::ANDROID_LOG_WARN, __VA_ARGS__)
    #define  LOGE(...)  __android_log_print(purple::ANDROID_LOG_ERROR, __VA_ARGS__)

    void Log::e(const std::string &tag , std::string msg){
        LOGE(tag.c_str() , "%s" ,msg.c_str());
    }

    void Log::w(const std::string &tag , std::string msg){
        LOGW(tag.c_str() , "%s" ,msg.c_str());
    }

    void Log::i(const std::string &tag , std::string msg){
        LOGI(tag.c_str() , "%s" ,msg.c_str());
    }
#endif

    void Log::e(const std::string &tag , std::string msg){
        printf(LIGHT_RED"%s[%s]:%s\n" NONE ,currentShowTime().c_str(), tag.c_str() , msg.c_str());
    }

    void Log::w(const std::string &tag , std::string msg){
        printf(YELLOW"%s[%s]:%s\n" NONE ,currentShowTime().c_str(), tag.c_str() , msg.c_str());
    }

    void Log::i(const std::string &tag , std::string msg){
        printf(WHITE"%s[%s]:%s\n" NONE ,currentShowTime().c_str(), tag.c_str() , msg.c_str());
    }

    void Log::purple(const std::string &tag , std::string msg){
        printf(PURPLE"%s[%s]:%s\n" NONE ,currentShowTime().c_str(), tag.c_str() , msg.c_str());
    }

    void Log::blue(const std::string &tag , std::string msg){
        printf(BLUE"%s[%s]:%s\n" NONE ,currentShowTime().c_str(), tag.c_str() , msg.c_str());
    }

    void Log::brown(const std::string &tag , std::string msg){
        printf(BROWN"%s[%s]:%s\n" NONE ,currentShowTime().c_str(), tag.c_str() , msg.c_str());
    }
    
    std::string Log::currentShowTime(){
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto str = std::put_time(std::localtime(&t), "%X");
        std::stringstream ss;
        ss << str;
        return ss.str();
    }
}