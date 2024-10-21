#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Component.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
//#include "D2DGraphic.h"
#include "Interval.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif


using namespace std;
namespace yunutyEngine
{
    class BoxCollider2D;
    class CircleCollider2D;
    class LineCollider2D;
    class RigidBody2D;
    class YUNUTY_API Collider2D abstract : public Component
    {
        // 쿼드트리 최적화 나중에 구현하기
    public:
        struct YUNUTY_API QuadTreeNode
        {
            static unique_ptr<QuadTreeNode> rootNode;
            double GetArea() { return xInterval.GetLength() * yInterval.GetLength(); }
            Interval xInterval;
            Interval yInterval;
            unique_ptr<QuadTreeNode> leftTop;
            unique_ptr<QuadTreeNode> rightTop;
            unique_ptr<QuadTreeNode> leftBottom;
            unique_ptr<QuadTreeNode> rightBottom;
            vector<Collider2D*> colliders;
        };
        const unordered_set<Collider2D*>& GetOverlappedColliders() const;
        virtual double GetArea() const = 0;
        virtual bool isOverlappingWith(const Collider2D* other) const = 0;
        virtual bool isOverlappingWith(const BoxCollider2D* other) const = 0;
        virtual bool isOverlappingWith(const CircleCollider2D* other) const = 0;
        virtual bool isOverlappingWith(const LineCollider2D* other) const = 0;
    private:
        // called by yunuty cycle
        static void InvokeCollisionEvents();
    protected:
        static unordered_set<Collider2D*> colliders2D;
        unordered_set<Collider2D*> overlappedColliders;

        Collider2D();
        virtual ~Collider2D();

        virtual bool isInsideNode(const QuadTreeNode* node)const = 0;
        static bool isOverlapping(const BoxCollider2D* a, const BoxCollider2D* b);
        static bool isOverlapping(const BoxCollider2D* a, const CircleCollider2D* b);
        static bool isOverlapping(const CircleCollider2D* b, const BoxCollider2D* a) { return isOverlapping(a, b); }
        static bool isOverlapping(const BoxCollider2D* a, const LineCollider2D* b);
        static bool isOverlapping(const LineCollider2D* b, const BoxCollider2D* a) { return isOverlapping(a,b); }
        static bool isOverlapping(const CircleCollider2D* a, const CircleCollider2D* b);
        static bool isOverlapping(const CircleCollider2D* a, const LineCollider2D* b);
        static bool isOverlapping(const LineCollider2D* b, const CircleCollider2D* a) { return isOverlapping(a,b); }
        static bool isOverlapping(const LineCollider2D* a, const LineCollider2D* b);

        friend YunutyCycle;
    };
}