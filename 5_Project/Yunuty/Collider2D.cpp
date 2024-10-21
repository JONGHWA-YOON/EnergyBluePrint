#include "YunutyEngine.h"
#include "Collision2D.h"

#include "Collider2D.h"

using namespace yunutyEngine;

unordered_set<Collider2D*> Collider2D::colliders2D;
unique_ptr<Collider2D::QuadTreeNode> Collider2D::QuadTreeNode::rootNode = nullptr;

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};
struct PairEqual {
    template <class T1, class T2>
    bool operator()(const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2) const {
        return p1.first == p2.first && p1.second == p2.second;
    }
};

Collider2D::Collider2D()
{
    colliders2D.insert(this);
}
Collider2D::~Collider2D()
{
    colliders2D.erase(this);
}
void Collider2D::InvokeCollisionEvents()
{
    // 쿼드트리 구성
    {
        const double k = 7;
        QuadTreeNode::rootNode = make_unique<QuadTreeNode>();
        double top = 0, bottom = 0, left = 0, right = 0;
        double meanColliderArea = 0;
        bool rectInitialized = false;
        auto activeColliders = colliders2D;
        erase_if(activeColliders, [](Collider2D* each) {return !each->GetGameObject()->GetActive(); });
        for (auto collider : activeColliders)
        {
            meanColliderArea += collider->GetArea();
            auto pos = collider->GetTransform()->GetWorldPosition();
            if (!rectInitialized)
            {
                left = right = pos.x;
                top = bottom = pos.y;
                rectInitialized = true;
                continue;
            }

            if (pos.x > right)
                right = pos.x;
            if (pos.x < left)
                left = pos.x;
            if (pos.y > top)
                top = pos.y;
            if (pos.y < bottom)
                bottom = pos.y;
        }
        meanColliderArea /= activeColliders.size();

        QuadTreeNode::rootNode->xInterval = Interval(left, right);
        QuadTreeNode::rootNode->yInterval = Interval(bottom, top);
        QuadTreeNode::rootNode->colliders = vector<Collider2D*>(activeColliders.begin(), activeColliders.end());
        stack<QuadTreeNode*> quadTreesStack;
        quadTreesStack.push(QuadTreeNode::rootNode.get());
        while (!quadTreesStack.empty())
        {
            auto node = quadTreesStack.top();
            quadTreesStack.pop();
            if (node->GetArea() < meanColliderArea * k)
                continue;
            if (node->colliders.size() < k)
                continue;

            Vector2d center((node->xInterval.left + node->xInterval.right) / 2, (node->yInterval.left + node->yInterval.right) / 2);
            node->leftTop = make_unique<QuadTreeNode>();
            node->rightTop = make_unique<QuadTreeNode>();
            node->leftBottom = make_unique<QuadTreeNode>();
            node->rightBottom = make_unique<QuadTreeNode>();
            node->leftTop->xInterval = Interval(node->xInterval.left, center.x);
            node->leftTop->yInterval = Interval(center.y, node->yInterval.right);
            node->rightTop->xInterval = Interval(center.x, node->xInterval.right);
            node->rightTop->yInterval = Interval(center.y, node->yInterval.right);
            node->leftBottom->xInterval = Interval(node->xInterval.left, center.x);
            node->leftBottom->yInterval = Interval(node->yInterval.left, center.y);
            node->rightBottom->xInterval = Interval(center.x, node->xInterval.right);
            node->rightBottom->yInterval = Interval(node->yInterval.left, center.y);
            for (QuadTreeNode* eachNode : { node->leftTop.get(),node->rightTop.get(),node->leftBottom.get(),node->rightBottom.get() })
            {
                for (auto eachCollider : node->colliders)
                    if (eachCollider->isInsideNode(eachNode))
                        eachNode->colliders.push_back(eachCollider);
                quadTreesStack.push(eachNode);
            }
        }
    }
    unordered_set<pair<Collider2D*, Collider2D*>, PairHash, PairEqual> collisionCheckboard;

    stack<QuadTreeNode*> quadTreeStack;
    quadTreeStack.push(QuadTreeNode::rootNode.get());
    while (!quadTreeStack.empty())
    {
        auto node = quadTreeStack.top();
        quadTreeStack.pop();

        if (node->leftTop.get() != nullptr)
        {
            quadTreeStack.push(node->leftTop.get());
            quadTreeStack.push(node->rightTop.get());
            quadTreeStack.push(node->leftBottom.get());
            quadTreeStack.push(node->rightBottom.get());
            continue;
        }
        // 말단 노드인 경우 충돌체크를 진행한다.
        for (Collider2D* colliderA : node->colliders)
            for (Collider2D* colliderB : node->colliders)
            {
                if (colliderA == colliderB)
                    continue;
                if (collisionCheckboard.find((colliderA < colliderB) ? make_pair(colliderA, colliderB) : make_pair(colliderB, colliderA)) == collisionCheckboard.end())
                {
                    if (colliderB->isOverlappingWith(colliderA))
                    {
                        Collision2D collisionOnA;
                        collisionOnA.m_Collider = colliderA;
                        collisionOnA.m_Rigidbody = colliderA->GetGameObject()->GetComponent<RigidBody2D>();
                        collisionOnA.m_OtherCollider = colliderB;
                        collisionOnA.m_OtherRigidbody = colliderB->GetGameObject()->GetComponent<RigidBody2D>();

                        Collision2D collisionOnB;
                        collisionOnB.m_Collider = colliderB;
                        collisionOnB.m_Rigidbody = colliderB->GetGameObject()->GetComponent<RigidBody2D>();
                        collisionOnB.m_OtherCollider = colliderA;
                        collisionOnB.m_OtherRigidbody = colliderA->GetGameObject()->GetComponent<RigidBody2D>();

                        if (colliderA->overlappedColliders.find(colliderB) == colliderA->overlappedColliders.end())
                        {
                            colliderA->overlappedColliders.insert(colliderB);
                           for (auto each : colliderA->GetGameObject()->GetIndexedComponents())
                                each->OnCollisionEnter2D(collisionOnA);

                            colliderB->overlappedColliders.insert(colliderA);
                            for (auto each : colliderB->GetGameObject()->GetIndexedComponents())
                                each->OnCollisionEnter2D(collisionOnB);
                        }
                        else
                        {
                            for (auto each : colliderA->GetGameObject()->GetIndexedComponents())
                                each->OnCollisionStay2D(collisionOnA);
                            for (auto each : colliderB->GetGameObject()->GetIndexedComponents())
                                each->OnCollisionStay2D(collisionOnB);
                        }
                    }
                    else
                    {
                        if (colliderA->overlappedColliders.find(colliderB) != colliderA->overlappedColliders.end())
                        {
                            Collision2D collisionOnA;
                            collisionOnA.m_Collider = colliderA;
                            collisionOnA.m_Rigidbody = colliderA->GetGameObject()->GetComponent<RigidBody2D>();
                            collisionOnA.m_OtherCollider = colliderB;
                            collisionOnA.m_OtherRigidbody = colliderB->GetGameObject()->GetComponent<RigidBody2D>();

                            Collision2D collisionOnB;
                            collisionOnB.m_Collider = colliderB;
                            collisionOnB.m_Rigidbody = colliderB->GetGameObject()->GetComponent<RigidBody2D>();
                            collisionOnB.m_OtherCollider = colliderA;
                            collisionOnB.m_OtherRigidbody = colliderA->GetGameObject()->GetComponent<RigidBody2D>();

                            colliderA->overlappedColliders.erase(colliderB);
                            for (auto each : colliderA->GetGameObject()->GetIndexedComponents())
                                each->OnCollisionExit2D(collisionOnA);

                            colliderB->overlappedColliders.erase(colliderA);
                            for (auto each : colliderB->GetGameObject()->GetIndexedComponents())
                                each->OnCollisionExit2D(collisionOnB);
                        }
                    }
                    collisionCheckboard.insert((colliderA < colliderB) ? make_pair(colliderA, colliderB) : make_pair(colliderB, colliderA));
                }
            }
    }
}
bool Collider2D::isOverlapping(const BoxCollider2D* a, const BoxCollider2D* b)
{
    auto angleA = a->GetTransform()->GetWorldRotation().Euler().z * math::Deg2Rad;
    auto angleB = b->GetTransform()->GetWorldRotation().Euler().z * math::Deg2Rad;

    // 사각형의 각 변을 평면공간위에 표시한 벡터
    Vector2d horizontalAxisA(cos(angleA), sin(angleA));
    Vector2d verticalAxisA(-sin(angleA), cos(angleA));
    Vector2d horizontalAxisB(cos(angleB), sin(angleB));
    Vector2d verticalAxisB(-sin(angleB), cos(angleB));

    auto axes = { horizontalAxisA,verticalAxisA,horizontalAxisB,verticalAxisB };
    for (const Vector2d& each : axes)
    {
        Interval aInterval = a->projectedInterval(each);
        Interval bInterval = b->projectedInterval(each);
        if (!aInterval.OverlappedWith(bInterval))
            return false;
    }

    return true;
}
bool Collider2D::isOverlapping(const BoxCollider2D* a, const CircleCollider2D* b)
{
    auto angleA = a->GetTransform()->GetWorldRotation().Euler().z * math::Deg2Rad;
    auto positionA = a->GetTransform()->GetWorldPosition();
    auto positionB = b->GetTransform()->GetWorldPosition();

    auto scale = a->GetTransform()->GetWorldScale();
    auto scaledHeight = a->collisonRect.height * scale.y;
    auto scaledWidth = a->collisonRect.width * scale.x;
    auto innerAngleA = atan(scaledHeight / scaledWidth);

    // 사각형의 각 변을 평면공간위에 표시한 벡터
    Vector2d horizontalAxisA(cos(angleA), sin(angleA));
    Vector2d verticalAxisA(-sin(angleA), cos(angleA));
    Vector2d rectPoints[4];
    Vector2d circleDisplacements[4];

    rectPoints[0] = positionA + horizontalAxisA * 0.5 * scaledWidth + verticalAxisA * 0.5 * scaledHeight;
    rectPoints[1] = positionA + horizontalAxisA * 0.5 * scaledWidth - verticalAxisA * 0.5 * scaledHeight;
    rectPoints[2] = positionA - horizontalAxisA * 0.5 * scaledWidth + verticalAxisA * 0.5 * scaledHeight;
    rectPoints[3] = positionA - horizontalAxisA * 0.5 * scaledWidth - verticalAxisA * 0.5 * scaledHeight;
    circleDisplacements[0] = rectPoints[0] - positionB;
    circleDisplacements[1] = rectPoints[1] - positionB;
    circleDisplacements[2] = rectPoints[2] - positionB;
    circleDisplacements[3] = rectPoints[3] - positionB;

    circleDisplacements[0] = circleDisplacements[0].Normalized();
    circleDisplacements[1] = circleDisplacements[1].Normalized();
    circleDisplacements[2] = circleDisplacements[2].Normalized();
    circleDisplacements[3] = circleDisplacements[3].Normalized();

    //vector<Vector2d> axes;
    //for (int i = 0; i < 360; i++)
        //axes.push_back(Vector2d::DirectionByAngle(i * YunutyMath::Deg2Rad));
    auto axes = { horizontalAxisA,verticalAxisA,circleDisplacements[0],circleDisplacements[1],circleDisplacements[2],circleDisplacements[3] };
    for (const Vector2d& each : axes)
    {
        Interval aInterval = a->projectedInterval(each);
        Interval bInterval = b->projectedInterval(each);
        if (!aInterval.OverlappedWith(bInterval))
            return false;
    }
    return true;
}
const unordered_set<Collider2D*>& Collider2D::GetOverlappedColliders() const
{
    return overlappedColliders;
}

bool Collider2D::isOverlapping(const BoxCollider2D* a, const LineCollider2D* b)
{
    return false;
}

bool Collider2D::isOverlapping(const CircleCollider2D* a, const CircleCollider2D* b)
{
    Vector2d aPos = a->GetTransform()->GetWorldPosition();
    Vector2d bPos = b->GetTransform()->GetWorldPosition();
    return (aPos - bPos).Magnitude() < a->radius + b->radius;
}

bool Collider2D::isOverlapping(const CircleCollider2D* a, const LineCollider2D* b)
{
    return false;
}

bool Collider2D::isOverlapping(const LineCollider2D* a, const LineCollider2D* b)
{
    return false;
}


