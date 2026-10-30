#pragma once

#include <string>
#include "application.h"
#include "utils/log.h"
#include "utils/random.h"
#include "utils/format.h"
#include "renderer/render.h"
#include "resource/asset_manager.h"
#include "widget/task_schedule.h"
#include "widget/thread_pool.h"
#include "input/input_manager.h"
#include "audio/audio_manager.h"
#include "renderer/pipeline/pipe_common.h"

namespace zidian{
    std::string Version();
    std::string Name();
    std::string Author();

    extern Application* Instance;
}


