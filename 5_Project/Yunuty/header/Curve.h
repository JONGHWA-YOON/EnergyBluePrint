#pragma once
#include <vector>
#include <functional>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    namespace math
    {
        // 다항식을 통해 2차원 공간의 곡선그래프를 그릴 수 있는 커브를 나타냅니다.
        class YUNUTY_API Curve
        {
        private:
            // curveFunction에 따라 Evaluate 함수의 반환 값이 달라집니다.
            function<double(double)> curveFunction;
        public:
            Curve() :Curve([](double t) {return t; }) {}
            Curve(const function<double(double)> curveFunction) :curveFunction(curveFunction) {}
            double Evaluate(double t) const { return curveFunction(t); }
        };
        // Linear : 일직선 y=x
        extern const Curve Linear;
        // 속도가 빨라지다가 갑자기 물체가 멈추는 모습을 표현하기에 적절한 커브입니다.
        extern const Curve EaseInQuad;
        // 초기속도가 빠른 상태에서 속도가 줄어들며 서서히 물체가 멈추는 모습을 표현하기에 적절한 커브입니다.
        extern const Curve EaseOutQuad;
        // 초기속도가 빨라지다가 다시 속도가 줄어들며 서서히 물체가 멈추는 모습을 표현하기에 적절한 커브입니다. 
        extern const Curve easeInOutQuad;
    };
}
