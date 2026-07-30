#include "utils/random.h"

namespace zidian{
    namespace RandomUtil{
        std::default_random_engine rndEngine;

        void setRandomSeed(int seed){
            rndEngine.seed(seed);
            static std::uniform_int_distribution<int> u;
            u(rndEngine);
        }
        
        int randomInt(int min, int max){
            static std::uniform_int_distribution<int> u(min , max);
            return u(rndEngine);
        }
        
        float randomFloat(float min , float max){
            static std::uniform_real_distribution<float> u(min, max);
            return u(rndEngine);
        }
    }
}
