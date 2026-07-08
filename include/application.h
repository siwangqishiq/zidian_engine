#pragma once

#include <string>
#include <cstdint>

namespace zidian{
    struct AppConfig{
        std::string name;
        uint32_t windowWidth;
        uint32_t windowHeight;
    };

    class Application{
    public:
        static std::string TAG;
        
        Application();

        void init(AppConfig config);

        int execute();

        ~Application();
    private:
        AppConfig config;
    };
}