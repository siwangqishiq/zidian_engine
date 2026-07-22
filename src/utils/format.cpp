#include "utils/format.h"
#include <cmath>
#include <iomanip>

namespace zidian {
    namespace Utils{
        const char* units[] = {
            "B", "KB", "MB", "GB", "TB", "PB"
        };
        
        std::string FormatBytes(uint64_t bytes, int precision){
            double size = static_cast<double>(bytes);
            int unit = 0;

            while (size >= 1024.0f && unit < 5){
                size /= 1024.0f;
                ++unit;
            }//end while

            std::ostringstream oss;

            if (unit == 0) {
                oss << bytes << units[unit];
            }else{
                double rounded = std::round(size);
                if(std::fabs(size - rounded) < 0.05){
                    oss << static_cast<uint64_t>(rounded);
                }else{
                    oss << std::fixed << std::setprecision(precision) << size;
                }
                oss << units[unit];
            }
            return oss.str();
        }


    }
}

