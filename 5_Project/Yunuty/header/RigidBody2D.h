#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
//#include "D2DGraphic.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif
using namespace std;
namespace yunutyEngine
{
    class Collision2D;
    class YUNUTY_API RigidBody2D : public Component
    {
    private:
        Vector2d cachedVelocity;
        bool isVelocityCached=false;
    public:
        Vector2d position;
        double rotation=0;
        Vector2d velocity;
        double angularVelocity=0;
        bool useAutoMass=false;
        double mass = 1;
        Vector2d centerOfMass;
    protected:
        void Update() override;
        virtual void OnCollisionEnter2D(const Collision2D& collision)override;
    };
}