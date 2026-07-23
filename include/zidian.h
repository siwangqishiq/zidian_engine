#pragma once

#include <string>
#include "application.h"
#include "utils/log.h"
#include "renderer/render.h"
#include "resource/asset_manager.h"
#include "widget/task_schedule.h"


namespace zidian{
    std::string Version();
    std::string Name();

    extern Application* Instance;
}


