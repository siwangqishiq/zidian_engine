#pragma once

#include "vulkan/vulkan.h"
#include "renderer/canvas.h"

namespace zidian{
    class Application;
    
    class Render : public ICanvas{
    public:
        Render(Application *appContext);

        void initVulkan();

        void onDispose();

        void beginRenderFrame();

        void endRenderFrame();

        void drawTriangles();
    private:
        VkInstance instance;

        void createInstance();

        Application *appCtx;
    };
}