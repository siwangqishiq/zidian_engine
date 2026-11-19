#include "zidian.h"

class TestDrawBezier : public zidian::IGame {
public:
    virtual void onInit() override{
        zidian::Log::brown("TestDrawBezier", "TestDrawBezier init");
    }
    
    virtual void onTick(float deltaTime) override{
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        const float screenWidth = static_cast<float>(canvas->getWidth());
        const float screenHeight = static_cast<float>(canvas->getHeight());

        testDrawBezier2Case();
        testDrawBezier3Case();
    }


    void testDrawBezier3Case(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        // 1. 平缓对称曲线
        canvas->drawBezier3(
            {100.0f, 100.0f},
            {200.0f, 0.0f},
            {400.0f, 0.0f},
            {500.0f, 100.0f},
            4.0f,
            {1.0f, 0.0f, 0.0f, 1.0f}
        );

        // 2. 大弧度曲线
        canvas->drawBezier3(
            {100.0f, 250.0f},
            {150.0f, 50.0f},
            {450.0f, 50.0f},
            {500.0f, 250.0f},
            5.0f,
            {0.0f, 1.0f, 0.0f, 1.0f}
        );

        // 3. S 型曲线
        canvas->drawBezier3(
            {100.0f, 400.0f},
            {400.0f, 100.0f},
            {200.0f, 700.0f},
            {500.0f, 400.0f},
            5.0f,
            {0.0f, 0.0f, 1.0f, 1.0f}
        );

        // 4. 向下弯曲的 S 型曲线
        canvas->drawBezier3(
            {600.0f, 100.0f},
            {900.0f, 400.0f},
            {700.0f, 500.0f},
            {1100.0f, 700.0f},
            5.0f,
            {1.0f, 0.0f, 1.0f, 1.0f}
        );

        // 5. 不对称曲线
        canvas->drawBezier3(
            {100.0f, 600.0f},
            {150.0f, 200.0f},
            {400.0f, 750.0f},
            {550.0f, 500.0f},
            6.0f,
            {0.0f, 1.0f, 1.0f, 1.0f}
        );

        // 6. 近似直线
        canvas->drawBezier3(
            {600.0f, 100.0f},
            {750.0f, 95.0f},
            {950.0f, 105.0f},
            {1100.0f, 100.0f},
            4.0f,
            {1.0f, 1.0f, 0.0f, 1.0f}
        );

        // 7. 急剧弯曲
        canvas->drawBezier3(
            {650.0f, 400.0f},
            {1100.0f, 400.0f},
            {650.0f, 700.0f},
            {1100.0f, 700.0f},
            8.0f,
            {1.0f, 0.5f, 0.0f, 1.0f}
        );

        // 8. 控制点交叉（测试复杂曲线形态）
        canvas->drawBezier3(
            {100.0f, 700.0f},
            {500.0f, 100.0f},
            {100.0f, 100.0f},
            {500.0f, 700.0f},
            6.0f,
            {0.5f, 0.0f, 1.0f, 1.0f}
        );

        // 9. 粗线条测试
        canvas->drawBezier3(
            {600.0f, 200.0f},
            {700.0f, 50.0f},
            {1000.0f, 50.0f},
            {1100.0f, 200.0f},
            20.0f,
            {0.0f, 0.5f, 1.0f, 1.0f}
        );

        // 10. 半透明曲线
        canvas->drawBezier3(
            {100.0f, 300.0f},
            {350.0f, 50.0f},
            {450.0f, 550.0f},
            {700.0f, 300.0f},
            20.0f,
            {1.0f, 0.0f, 0.0f, 0.5f}
        );

        canvas->drawBezier3(
            {300.0f, 300.0f},
            {550.0f, 550.0f},
            {650.0f, 50.0f},
            {900.0f, 300.0f},
            20.0f,
            {0.0f, 0.0f, 1.0f, 0.5f}
        );
    }

    void testDrawBezier2Case(){
        auto &render = zidian::Instance->getRender();
        auto &canvas = render->getCanvas();
        
        const float screenWidth = static_cast<float>(canvas->getWidth());
        const float screenHeight = static_cast<float>(canvas->getHeight());

        canvas->drawBezier2(
            {100, 100},
            {300, 0},
            {500, 100},
            4.0f,
            {1, 0, 0, 1}
        );

        // 2. 大弧度曲线
        canvas->drawBezier2(
            {100.0f, 250.0f},
            {300.0f, 50.0f},
            {500.0f, 250.0f},
            4.0f,
            {0.0f, 1.0f, 0.0f, 1.0f}
        );

        // 3. 向下弯曲
        canvas->drawBezier2(
            {600.0f, 100.0f},
            {800.0f, 300.0f},
            {1000.0f, 100.0f},
            4.0f,
            {0.0f, 0.0f, 1.0f, 1.0f}
        );

        // 4. 近似直线
        canvas->drawBezier2(
            {100.0f, 400.0f},
            {300.0f, 395.0f},
            {500.0f, 400.0f},
            4.0f,
            {1.0f, 1.0f, 0.0f, 1.0f}
        );

        // 5. 不对称大弧度曲线
        canvas->drawBezier2(
            {600.0f, 400.0f},
            {900.0f, 100.0f},
            {1100.0f, 500.0f},
            4.0f,
            {1.0f, 0.0f, 1.0f, 1.0f}
        );

        // 6. 大幅度向上弯曲
        canvas->drawBezier2(
            {100.0f, 700.0f},
            {300.0f, 300.0f},
            {500.0f, 700.0f},
            6.0f,
            {0.0f, 1.0f, 1.0f, 1.0f}
        );

        // 7. 不同线宽测试
        canvas->drawBezier2(
            {650.0f, 200.0f},
            {850.0f, 50.0f},
            {1050.0f, 200.0f},
            1.0f,
            {1.0f, 0.0f, 0.0f, 1.0f}
        );

        canvas->drawBezier2(
            {650.0f, 250.0f},
            {850.0f, 100.0f},
            {1050.0f, 250.0f},
            10.0f,
            {0.0f, 1.0f, 0.0f, 1.0f}
        );

        canvas->drawBezier2(
            {650.0f, 320.0f},
            {850.0f, 170.0f},
            {1050.0f, 320.0f},
            20.0f,
            {0.0f, 0.0f, 1.0f, 1.0f}
        );

        // 8. 半透明曲线交叉测试
        canvas->drawBezier2(
            {100.0f, 500.0f},
            {400.0f, 200.0f},
            {700.0f, 500.0f},
            20.0f,
            {1.0f, 0.0f, 0.0f, 0.5f}
        );

        canvas->drawBezier2(
            {400.0f, 300.0f},
            {400.0f, 700.0f},
            {800.0f, 300.0f},
            20.0f,
            {0.0f, 0.0f, 1.0f, 0.5f}
        );
    }

    virtual void onDispose() override {
        zidian::Log::brown("game", "game dispose");
    }
};


void TestDrawBezierCase(){
    zidian::AppConfig config;
    config.name = "Test Draw Lines";
    config.windowWidth = 1280;
    config.windowHeight = 800;
    config.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    config.vsync = true;
    config.isRecordLog = true;

    zidian::Application app;
    app.setGameObject(std::make_shared<TestDrawBezier>());
    app.init(config);
    app.execute();
}




