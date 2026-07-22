#pragma once

namespace zidian {
    class Render;

    class PrimitiveBatch {
    public:
        PrimitiveBatch(Render &context);

        ~PrimitiveBatch() = default;

        void flush();
    private:
        Render &ctx;
    };
}



