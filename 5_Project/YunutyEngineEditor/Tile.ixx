module;
#include "YunutyEngine.h"
#include <math.h>
export module htkg.InGame.Tile;
import htkg.InGame.Robot;
import htkg.InGame.Laser;
import htkg.InGame.Battery;
import htkg.InGame.Button;
import htkg.InGame.Destination;

// 모듈에서는 전방선언도 export 해야됨
export class Stage;

// Serialize 대상임
export class Tile : public yunutyEngine::Component
{
public:
    //const float TileHeightFromPivot = 1;
    void ApplyChanges()
    {
        // 타일타입이 같은데 속성이 좀 바뀐 경우
        if (lastTileType == tileType)
        {
            if (tileType == TileType::Robot)
                _robot->energy = energy;
            // 레이저 방향 돌리기
            if (tileType == TileType::Laser)
            {
                _laser->GetTransform()->SetWorldRotation(Vector3d(0, (int)LaserDirection * 90, 0));
            }
            lastLaserDirection = LaserDirection;
            return;
        }
        // 타일타입 자체가 바뀐 경우 이전 타일의 흔적을 지운다.
        switch (lastTileType)
        {
        case Tile::TileType::None:
            break;
        case Tile::TileType::Blocked:
            tileMesh->SetActive(true);
            break;
        case Tile::TileType::Robot:
            Scene::getCurrentScene()->DestroyGameObject(_robot->GetGameObject());
            _robot = nullptr;
            break;
        case Tile::TileType::Battery:
            Scene::getCurrentScene()->DestroyGameObject(_battery->GetGameObject());
            _battery = nullptr;
            break;
        case Tile::TileType::Laser:
            Scene::getCurrentScene()->DestroyGameObject(_laser->GetGameObject());
            _laser = nullptr;
            break;
        case Tile::TileType::Button:
            Scene::getCurrentScene()->DestroyGameObject(_button->GetGameObject());
            _button = nullptr;
            break;
        case Tile::TileType::Destination:
            Scene::getCurrentScene()->DestroyGameObject(_destination->GetGameObject());
            _destination = nullptr;
            break;
        default:
            break;
        }
        // 타일타입 자체가 바뀐 경우 새 타일에 맞게 객체를 생성한다.
        switch (tileType)
        {
        case Tile::TileType::None:
            break;
        case Tile::TileType::Blocked:
            tileMesh->SetActive(false);
            break;
        case Tile::TileType::Robot:
            _robot = GetGameObject()->AddGameObject()->AddComponent<Robot>();
            _robot->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
            _robot->currentTile = this;
            break;
        case Tile::TileType::Battery:
            _battery = GetGameObject()->AddGameObject()->AddComponent<Battery>();
            _battery->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
            break;
        case Tile::TileType::Laser:
            _laser = GetGameObject()->AddGameObject()->AddComponent<Laser>();
            _laser->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
            break;
        case Tile::TileType::Button:
            _button = GetGameObject()->AddGameObject()->AddComponent<Button>();
            _button->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
            break;
        case Tile::TileType::Destination:
            _destination = GetGameObject()->AddGameObject()->AddComponent<Destination>();
            _destination->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
            break;
        default:
            break;
        }
        lastTileType = tileType;
    }
    bool isAdjacent(const Tile* tile)
    {
        return abs(tile->_row - _row) + abs(tile->_col - _col) == 1;
    }

    enum class TileType
    {
        None, Blocked, Robot, Battery, Laser, Button, Destination
    };
    enum class Direction
    {
        Up = 0, Right, Down, Left,
    };
    // Serialize 대상임
    TileType tileType{ TileType::None };
    int energy{ 0 };
    int mechanismIndex{ 0 };
    Direction LaserDirection{ Direction::Up };

    //constexpr static float standardScale = 0.0065;
    //constexpr static float standardScaleInv = 1/0.0065;
    // Serialize 대상이 아님.
    int _row{ 0 }, _col{ 0 };
    graphics::StaticMesh* tileMesh{ nullptr };
    Stage* _stage{ nullptr };
    bool _isDangerous{ false };
    Robot* _robot{ nullptr };
    Battery* _battery{ nullptr };
    Button* _button{ nullptr };
    Destination* _destination{ nullptr };
    Laser* _laser{ nullptr };
private:
    TileType lastTileType{ TileType::None };
    Direction lastLaserDirection{ Direction::Up };
};