#pragma once

#include <random>

namespace zidian{
    namespace RandomUtil{
        extern std::default_random_engine rndEngine;

        void setRandomSeed(int seed);
        int randomInt(int min, int max);
        float randomFloat(float min, float max);
    };
}


