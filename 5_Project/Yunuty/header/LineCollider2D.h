#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
//#include "D2DGraphic.h"
#include "Collider2D.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    class YUNUTY_API LineCollider2D : public Collider2D
    {
        friend Collider2D;
    private:
        virtual bool isOverlappingWith(const BoxCollider2D* other)const override;
        virtual bool isOverlappingWith(const CircleCollider2D* other)const override;
        virtual bool isOverlappingWith(const LineCollider2D* other)const override;
    };
}
