#include <chrono>
#include <sstream>
#include <iomanip>
#include "utils/log.h"
#include "utils/time.h"

namespace zidian {
    const std::string Log::TAG = "log";
    const std::string Log::TIME_FORMAT = "%H:%M:%S";
    const std::string Log::DEFAULT_LOG_FILE_PATH = "zidian.log";


    using SystemClk = std::chrono::system_clock;

    std::ofstream Log::logFile;
    std::mutex Log::logMutex;

    void Log::e(const std::string &tag , std::string msg){
        write("ERROR",tag,msg,LIGHT_RED);
    }

    void Log::w(const std::string &tag , std::string msg){
        write("WARN",tag,msg,YELLOW);
    }

    void Log::i(const std::string &tag , std::string msg){
        write("INFO",tag,msg,WHITE);
    }

    void Log::purple(const std::string &tag , std::string msg){
        write("INFO",tag,msg,PURPLE);
    }

    void Log::green(const std::string &tag , std::string msg) {
        write("INFO",tag,msg,GREEN);
    }

    void Log::blue(const std::string &tag , std::string msg){
        write("INFO",tag,msg,BLUE);
    }

    void Log::brown(const std::string &tag , std::string msg){
        write("INFO",tag,msg,BROWN);
    }
    
    std::string Log::currentShowTime(){
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto str = std::put_time(std::localtime(&t), "%X");
        std::stringstream ss;
        ss << str;
        return ss.str();
    }

    void Log::setLogFile(const std::string& path){
        std::lock_guard<std::mutex> lock(logMutex);

        if (logFile.is_open()) {
            logFile.close();
        }

        logFile.open(path,std::ios::out | std::ios::app);

        if (!logFile.is_open()) {
#ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"Failed to open log file: %s",path.c_str());
#else
        printf("Failed to open log file: %s\n",path.c_str());
#endif
        }
    }

    void Log::closeLogFile(){
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open()){
            logFile.flush();
            logFile.close();
        }
    }

    void Log::write(const std::string& level, const std::string& tag,const std::string& msg,const char* color){
        std::lock_guard<std::mutex> lock(logMutex);
        writeToFile(level, tag, msg);

#ifdef __ANDROID__
    int priority = ANDROID_LOG_INFO;
    if (level == "ERROR") {
        priority = ANDROID_LOG_ERROR;
    } else if (level == "WARN") {
        priority = ANDROID_LOG_WARN;
    }
    __android_log_print(priority,tag.c_str(),"%s",msg.c_str());
#else
        printf("%s%s[%s]:%s\n",color,currentShowTime().c_str(),tag.c_str(),msg.c_str());
        printf("%s", NONE);
#endif

    }

    void Log::writeToFile(const std::string& level,const std::string& tag,const std::string& msg){
        if (!logFile.is_open()) {
            return;
        }

        logFile << currentShowTime() << " [" << level << "]"
            << " [" << tag << "] " << msg << std::endl;

        logFile.flush();
    }
}