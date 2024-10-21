#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Graphic.h"
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
    class YUNUTY_API RigidBody : public Component
    {
    public:
        Vector3f velocity;
        Vector3f angularVelocity;
        float drag;
        float angularDrag;
        float mass;
        bool useGravity;
        float maxDepenetrationVelocity;
        bool isKinematic;
    protected:
        void Update() override;
    };
}