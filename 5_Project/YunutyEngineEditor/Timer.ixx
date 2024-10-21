module;
#include <functional>
export module htkg.Timer;
//
//export std::function<void()>;
//export namespace std
//{
//    template<typename T>
//    class A;
//}
export class Timer
{
public:
    float elapsed = 0;		// 타이머가 시작된 후 경과된 시간
    bool isActive = false;
    bool isRepeat = false;
    bool useScaledTime = true;
    float duration = 1;		// 목표로하는 시간 ex) 3초 타이머를 하고 싶다
    std::function<void()> onExpiration = []() {};								// elapsed >= duration
    std::function<void(float)> onUpdate{ [](float progress) {} };
    void Update(float deltaTime)
    {
        if (isActive)
        {
            elapsed += deltaTime;

            if (elapsed > duration)
                elapsed = duration;

            if (onUpdate)
                onUpdate(elapsed / duration);

            if (elapsed == duration)
            {
                if (isRepeat == true)
                {
                    elapsed = 0;
                }
                else
                {
                    isActive = false;
                }
                onExpiration();
            }
        }
    }

    void Start()
    {
        elapsed = 0;
        isActive = true;
    }
};

