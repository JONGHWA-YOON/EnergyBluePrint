#include "YunutyEngine.h"
#include "YunutyTime.h"


double yunutyEngine::Time::timeScale = 1;
chrono::system_clock::time_point yunutyEngine::Time::lastFrameTime = (chrono::system_clock::time_point::min)();
double yunutyEngine::Time::deltaTimeUnscaled = 0;
queue<double> yunutyEngine::Time::fpsQueue;
void yunutyEngine::Time::Update()
{
    auto now = chrono::system_clock::now();
    //std::chrono::duration<double> diff = now - lastFrameTime;
    if (lastFrameTime != (chrono::system_clock::time_point::min)())
    {
        deltaTimeUnscaled = chrono::duration<double>(now - lastFrameTime).count();
#if _DEBUG
        // 디버그 중 중단점을 찍었을 때, deltaTimeUnscaled이 10초 이상이 될 때도 있다. 그 때를 대비한 코드
        if (deltaTimeUnscaled > 1)
            deltaTimeUnscaled = 0.000001;
#endif
    }

    lastFrameTime = now;
    auto nowInSeconds = ((chrono::duration<double>)now.time_since_epoch()).count();
    fpsQueue.push(nowInSeconds);
    while (!fpsQueue.empty() && fpsQueue.front() < nowInSeconds - 1)
    {
        fpsQueue.pop();
    }
}
void yunutyEngine::Time::SetTimeScale(const float& timeScale)
{
    Time::timeScale = timeScale;
}
double yunutyEngine::Time::GetTimeScale()
{
    return timeScale;
}
double yunutyEngine::Time::GetDeltaTime()
{
    return deltaTimeUnscaled * timeScale;
}
double yunutyEngine::Time::GetDeltaTimeUnscaled()
{
    return deltaTimeUnscaled;
}
int yunutyEngine::Time::GetFPS()
{
    return (int)fpsQueue.size();
}
