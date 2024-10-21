#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
#include "Interval.h"
//#include "D2DGraphic.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    class YUNUTY_API CircleCollider2D : public Collider2D
    {
        friend Collider2D;
    private:
        double radius=0;
    public:
        virtual double GetArea()const override;
        virtual bool isOverlappingWith(const Collider2D* other)const override;
        virtual bool isOverlappingWith(const BoxCollider2D* other)const override;
        virtual bool isOverlappingWith(const CircleCollider2D* other)const override;
        virtual bool isOverlappingWith(const LineCollider2D* other)const override;
        virtual bool isInsideNode(const QuadTreeNode* node)const override;
        Interval projectedInterval(const Vector2d& v)const;
        void SetRadius(double radius);
    };
}