#pragma once

#include <string>
#include "application.h"
#include "utils/log.h"
#include "utils/random.h"
#include "renderer/render.h"
#include "resource/asset_manager.h"
#include "widget/task_schedule.h"
#include "widget/thread_pool.h"
#include "input/input_manager.h"
#include "audio/audio_manager.h"

namespace zidian{
    std::string Version();
    std::string Name();
    
    extern Application* Instance;
}


