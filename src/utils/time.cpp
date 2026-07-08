#include "utils/time.h"

#include <chrono>

namespace zidian {
    long long CurrentTimeMicro(){
        using namespace std;
        using namespace std::chrono;
        auto now = system_clock::now();
        auto us = duration_cast<microseconds>(now.time_since_epoch());
        return us.count();
    }

    long long CurrentTimeMillis(){
        using namespace std;
        using namespace std::chrono;
        // get absolute wall time
        auto now = system_clock::now();
        // find the number of milliseconds
        auto ms = duration_cast<milliseconds>(now.time_since_epoch());
        return ms.count();
    }

    float CurrentTimeMillisFloat(){
        auto t = CurrentTimeMillis();
        return t / 1000.0f;
    }
}

