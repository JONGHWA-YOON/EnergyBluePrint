#pragma once
#include "Object.h"
#include <chrono>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif


using namespace std;
namespace yunutyEngine
{
    class YUNUTY_API Time :
        public Object
    {
        friend class YunutyCycle;
    private:
        static chrono::system_clock::time_point lastFrameTime;
        static double timeScale;
        static double deltaTimeUnscaled;
        static void Update();
        static queue<double> fpsQueue;
    public:
        static void SetTimeScale(const float& timeScale);
        static double GetTimeScale();
        static double GetDeltaTime();
        static double GetDeltaTimeUnscaled();
        static int GetFPS();
    };
}
