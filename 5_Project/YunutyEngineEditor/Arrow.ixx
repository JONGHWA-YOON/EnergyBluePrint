module;
#include "YunutyEngine.h"
#include <queue>;
export module htkg.InGame.Arrow;
import htkg.InGame.EffectParent;

export class Arrow : public Component
{
public:
    Arrow()
    {
        arrowBody = GetGameObject()->AddGameObject()->AddComponent<graphics::StaticMesh>();
        arrowBody->GetGameObject()->setName("arrow body");
        arrowBody->GetGI().LoadDiffuseMap("Textures/red.dds");
        arrowBody->GetGI().LoadMesh("FBXFile/frontRectangle.fbx");

        arrowHead = GetGameObject()->AddGameObject()->AddComponent<graphics::StaticMesh>();
        arrowHead->GetGameObject()->setName("arrow head");
        arrowHead->GetGI().LoadDiffuseMap("Textures/red.dds");
        arrowHead->GetGI().LoadMesh("FBXFile/frontTriangle.fbx");
    }
    void SetArrow(Vector3d origin, Vector3d dest, double arrowWidth)
    {
        float bodyLength = (origin - dest).Magnitude();
        arrowBody->GetTransform()->scale.z = bodyLength;
        arrowHead->GetTransform()->position.z = bodyLength;
        arrowBody->GetTransform()->scale.x = arrowWidth;
        arrowHead->GetTransform()->scale.x = arrowWidth;
        arrowHead->GetTransform()->scale.z = arrowWidth;
        arrowBody->Update();
        arrowHead->Update();
        GetTransform()->SetWorldPosition(origin);
        GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(dest - origin, Vector3d::up));
    }
    void SetAsTip(bool isTip)
    {
        arrowHead->GetGI().LoadMesh(isTip ? "FBXFile/frontTriangle.fbx" : "FBXFile/filledCircle.fbx");
    }
    static Arrow* CreateArrow(Vector3d origin, Vector3d dest, double arrowWidth)
    {
        auto arrow = EffectParent::GetEffectParent()->AddGameObject()->AddComponent<Arrow>();
        arrow->SetArrow(origin, dest, arrowWidth);
        return arrow;
    }
private:
    graphics::StaticMesh* arrowBody{ nullptr };
    graphics::StaticMesh* arrowHead{ nullptr };
};