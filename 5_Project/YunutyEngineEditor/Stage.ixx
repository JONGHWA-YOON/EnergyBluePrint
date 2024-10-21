module;
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "YunutyEngine.h"
#include "Timer.h"
#include <iostream>
#include <string.h>
#include <vector>
export module htkg.InGame.Stage;
import htkg.InGame.Battery;
import htkg.InGame.Button;
import htkg.InGame.Laser;
import htkg.InGame.Robot;
import htkg.InGame.Tile;
import htkg.ResourcesID;
import ui.DebugPanel;

//export class Tile;
// serialization 대상임
export class Stage : public yunutyEngine::Component
{
public:
    int GetHeight()
    {
        return m_goalHeightSize;
    }
    int GetWidth()
    {
        return m_goalWidthSize;
    }
    void SetHeight(int size)
    {
        m_goalHeightSize = size;
        SetBoard();
    }
    void SetWidth(int size)
    {
        m_goalWidthSize = size;
        SetBoard();
    }
    void SetBoard()
    {
        if (tiles.size() > m_goalHeightSize)
            for (int i = m_goalHeightSize; i < tiles.size(); i++)
                for (int j = 0; j < tiles[i].size(); j++)
                    Scene::getCurrentScene()->DestroyGameObject(tiles[i][j]->GetGameObject());

        if (tiles.size() > 0)
            if (tiles[0].size() > m_goalWidthSize)
                for (int i = m_goalWidthSize; i < tiles[0].size(); i++)
                    for (int j = 0; j < tiles.size(); j++)
                        Scene::getCurrentScene()->DestroyGameObject(tiles[j][i]->GetGameObject());

        if (m_prevBoardSizeH != m_goalHeightSize || m_prevBoardSizeW != m_goalWidthSize)
        {
            tiles.resize(m_goalHeightSize);
            for (int i = 0; i < tiles.size(); i++)
                tiles[i].resize(m_goalWidthSize);

            for (int i = 0; i < m_goalHeightSize; i++)
                for (int j = 0; j < m_goalWidthSize; j++)
                    if (tiles[i][j] == nullptr)
                        tiles[i][j] = AddTile(i, j);

            m_prevBoardSizeH = m_goalHeightSize;
            m_prevBoardSizeW = m_goalWidthSize;
        }

        if (m_prevInterval != m_interval)
        {
            SetInterval();
            m_prevInterval = m_interval;
        }
    }
    void SetInterval()
    {
        for (int i = 0; i < m_goalHeightSize; i++)
            for (int j = 0; j < m_goalWidthSize; j++)
                if (tiles[i][j] != nullptr)
                {
                    tiles[i][j]->GetTransform()->position.x = (m_interval + 1) * i;
                    tiles[i][j]->GetTransform()->position.z = (m_interval + 1) * j;
                }
    }

    // 시리얼라이즈 하시오
    int m_StageIndex = 0;
    // height = row, width = col
    int m_goalHeightSize = 0;
    int m_goalWidthSize = 0;
    float m_interval = 0.0f;
    // 매 턴이 수행되는 시간 간격
    constexpr static float ProceedInterval{ 0.5f };
    // 화살표의 y축 변위
    float arrowInterval{ 0.15f };
    float arrowWidth{ 0.05f };



    // 시리얼라이즈 하지 마시오
    bool isPlaying{ false };
    void ReportMouseHover(Tile* tile)
    {
        DebugPanel::LogMessage("hovered");
        if (isDraggingPath &&
            (tile->tileType == Tile::TileType::None ||
                tile->tileType == Tile::TileType::Button ||
                tile->tileType == Tile::TileType::Destination ||
                tile->tileType == Tile::TileType::Robot)

            && tile->isAdjacent(draggingPathRobot->moveQueue.back()))
        {
            // 되돌리고 싶을 경우
            if (draggingPathRobot->moveQueue.size() > 1 && tile == *(draggingPathRobot->moveQueue.rbegin() + 1))
            {
                DebugPanel::LogMessage("path revert");
                PopPath();
            }
            else
            {
                DebugPanel::LogMessage("path appended");
                AppendPath(tile);
            }
        }
    }
    void ReportMouseClick(Tile* tile)
    {
        if (!isDraggingPath && tile->tileType == Tile::TileType::Robot)
            StartDraggingPath(tile, tile->_robot);
    }
    void StartDraggingPath(Tile* origin, Robot* robot)
    {
        DebugPanel::LogMessage("dragging start");
        draggingPathRobot = robot;
        robot->ClearArrow();
        robot->moveQueue.clear();
        robot->moveQueue.push_back(origin);
        isDraggingPath = true;
    }
    void StopDraggingPath()
    {
        //draggingPathRobot->HideArrow();
        isDraggingPath = false;
    }
    void AppendPath(Tile* tile)
    {
        draggingPathRobot->AppendArrow(
            draggingPathRobot->moveQueue.back()->GetTransform()->GetWorldPosition() + Vector3d::up * arrowInterval,
            tile->GetTransform()->GetWorldPosition() + Vector3d::up * arrowInterval,
            arrowWidth
        );
        draggingPathRobot->moveQueue.push_back(tile);
    }
    void PopPath()
    {
        draggingPathRobot->PopArrow();
        draggingPathRobot->moveQueue.pop_back();
    }
    virtual void Update()override
    {
        std::stringstream sstr;
        sstr << "Stage " << m_StageIndex;
        string str;
        str = sstr.str();
        GetGameObject()->setName(str);
        if (Input::isKeyPushed(KeyCode::Space))
            StartGame();
        playingTimer.Update(Time::GetDeltaTime());
        if (isDraggingPath && Input::isKeyLifted(KeyCode::MouseLeftClick))
            StopDraggingPath();
    }
public:
    Tile* AddTile(int row, int col)
    {
        //Stage
        auto tileObj = GetGameObject()->AddGameObject();
        auto tile = tileObj->AddComponent<Tile>();
        tileObj->GetTransform()->position.x = (m_interval + 1) * row;
        tileObj->GetTransform()->position.z = (m_interval + 1) * col;
        //tileObj->GetTransform()->scale = Vector3d(Tile::standardScale, Tile::standardScale, Tile::standardScale);
        //tileObj->GetTransform()->rotation = Vector3d(-90, 0, 0);
        tile->_stage = this;
        tile->_row = row;
        tile->_col = col;
        std::string boardIndex = std::to_string(row) + "," + std::to_string(col);
        tileObj->setName("board " + boardIndex);
        tile->tileMesh = tileObj->AddComponent<yunutyEngine::graphics::StaticMesh>();
        tile->tileMesh->GetGI().LoadMesh(resourcesID::platformFBX.c_str());
        tile->tileMesh->GetGI().LoadDiffuseMap(resourcesID::platformDiffuseTex.c_str());
        tile->tileMesh->GetGI().LoadNormalMap(resourcesID::platformNormalTex.c_str());
        auto eventHandler = tileObj->AddComponent<yunutyEngine::graphics::EventHandler>();
        eventHandler->SetClickCallback([=]() {
            this->ReportMouseClick(tile);
            DebugPanel::LogMessage((tileObj->getName() + " clicked").c_str());
            });
        eventHandler->SetHoverCallback([=]() {
            this->ReportMouseHover(tile);
            DebugPanel::LogMessage((tileObj->getName() + " hovered").c_str());
            });
        return tile;
    }
    // 게임을 시작시키는 함수
    void StartGame()
    {
        if (isPlaying)
            return;
        isPlaying = true;
        playingTimer.isRepeat = true;
        playingTimer.duration = ProceedInterval;
        playingTimer.onExpiration = [=]() {this->Proceed(); };
        playingTimer.Start();
    }
    // 게임을 초기화하는 함수, 게임 재시작, 혹은 스테이지 로드때 호출된다.
    void ResetStage()
    {
        isPlaying = false;
        GetGameObject()->SetSelfActive(true);
        constexpr double perTile = 8.5 / 5.0;
        graphics::Camera::GetMainCamera()->GetTransform()->SetWorldPosition(GetCenterPosition() + Vector3d::up * perTile * fmax(m_goalHeightSize, m_goalWidthSize) - Vector3d::forward * m_goalWidthSize * 0.2);
        graphics::Camera::GetMainCamera()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(Vector3d::down, Vector3d::forward));
        robots.clear();
        buttons.clear();
        lasers.clear();
        batteries.clear();
        for (auto tile_col : tiles)
            for (auto tile : tile_col)
                ResetTile(tile);
        for (auto tile_col : tiles)
            for (auto tile : tile_col)
                switch (tile->tileType)
                {
                case Tile::TileType::Robot:
                    robots.push_back(tile->_robot);
                    break;
                case Tile::TileType::Button:
                    buttons[tile->mechanismIndex] = tile->_button;
                    break;
                case Tile::TileType::Laser:
                    lasers[tile->mechanismIndex] = tile->_laser;
                    break;
                case Tile::TileType::Battery:
                    batteries[tile->mechanismIndex] = tile->_battery;
                    break;
                default:
                    break;
                }
        for (auto each : buttons)
        {
            if (lasers.find(each.first) != lasers.end())
                each.second->target = lasers[each.first];
            if (batteries.find(each.first) != batteries.end())
                each.second->target = batteries[each.first];
        }
    }
    // 게임의 매 턴을 진행시키는 함수
    // 이건 코루틴을 써볼법도 한데...
    void Proceed();

    // 레이저로 인한 위험지대 재설정, 로봇들이 둘 이상 같은 위치에 있는지 체크하고 게임오버 시키기
    void EvaluateState()
    {
        SetAllTileSafe();
    }
    // 타일의 상태를 게임시작전 초기상태로 바꾼다.
    void ResetTile(Tile* tile)
    {
        SetAllTileSafe();
        tile->ApplyChanges();
        /*switch (tile->tileType)
        {
        case Tile::TileType::Robot:
            tile->_robot->GetTransform()->SetWorldPosition(tile->GetTransform()->GetWorldPosition());
            tile->_robot->currentTile = tile;
            tile->_robot->moveQueue.clear();
            tile->_robot->ClearArrow();
            tile->_robot->energy = tile->energy;
            robots.push_back(tile->_robot);
            break;
        case Tile::TileType::Laser:
            tile->_laser->active = true;
            break;
        case Tile::TileType::Battery:
            tile->_battery->remainingBattery = tile->energy;
            break;
        default:
            break;
        }*/
    }
    void SetAllTileSafe()
    {
        for (auto tile_col : tiles)
            for (auto tile : tile_col)
                tile->_isDangerous = false;
    }
    void SetDangerZone(Tile* laserTile)
    {
        // +col : up
        // +row : right
        int dr{ 0 }, dc{ 0 };
        int row = laserTile->_row, col = laserTile->_col;
        switch (laserTile->LaserDirection)
        {
        case Tile::Direction::Down:
            dc = -1;
            break;
        case Tile::Direction::Up:
            dc = +1;
            break;
        case Tile::Direction::Right:
            dr = +1;
            break;
        case Tile::Direction::Left:
            dr = -1;
            break;
        default:
            break;
        }
        while (0 <= row && row < this->m_goalHeightSize &&
            0 <= col && col < this->m_goalWidthSize)
            tiles[row][col]->_isDangerous = true;
    }
    Vector3d GetCenterPosition() { return GetTransform()->GetWorldPosition() + 0.5 * Vector3d(m_goalHeightSize, 0, m_goalWidthSize); }
    Timer playingTimer;
    std::vector<std::vector<Tile*>> tiles;
    std::vector<Robot*> robots;
    std::map<int, Button*> buttons;
    std::map<int, Laser*> lasers;
    std::map<int, Battery*> batteries;
    yunutyEngine::GameObject* parentObject = nullptr;
    int m_prevBoardSizeH = 0;
    int m_prevBoardSizeW = 0;
    float m_prevInterval = 1.1f;
    bool m_InitFlag = true;
    bool isDraggingPath{ false };
    Robot* draggingPathRobot{ nullptr };
    static Stage* currentStage;
    static void Reset() { currentStage->ResetStage(); };
    static void Run() { currentStage->StartGame(); };
    static void LoadNextStage(int currentStageIndex);
};
Stage* Stage::currentStage{nullptr};
