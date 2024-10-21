#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
//#include "D2DGraphic.h"
#include "Collider2D.h"
#include "Interval.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    class YUNUTY_API BoxCollider2D : public Collider2D
    {
        friend Collider2D;
    private:
        Rect collisonRect;
    protected:
        virtual bool isOverlappingWith(const Collider2D* other)const;
        virtual bool isOverlappingWith(const BoxCollider2D* other)const;
        virtual bool isOverlappingWith(const CircleCollider2D* other)const;
        virtual bool isOverlappingWith(const LineCollider2D* other)const;
        virtual bool isInsideNode(const QuadTreeNode* node)const override;
    public:
        virtual double GetArea()const override;
        void SetWidth(double width);
        void SetHeight(double height);
        double GetWidth()const;
        double GetHeight()const;
        Interval projectedInterval(const Vector2d& v)const;
    };
}