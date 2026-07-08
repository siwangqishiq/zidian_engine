#include <iostream>
#include "zidian.h"

void TestLogs(){
    const std::string tag = "zidian_log";
    zidian::Log::i(tag, "this is normal log %s", zidian::Name().c_str());
    zidian::Log::w(tag, "this is warning log");
    zidian::Log::e(tag, "this is error log");

    int a = 1;
    int b = 2;
    zidian::Log::w(tag, "%d + %d = %f", a, b , static_cast<float>(a + b));

    zidian::Log::purple("tag", "custom color purple");
    zidian::Log::blue("tag", "custom color blue");
    zidian::Log::brown("tag", "custom color brown");
}
