module;
#include <compare>
#include "YunutyEngine.h"
#include "YunutyGenericJsonHandler.h"
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
export module htkg.SaveLoad;
import htkg.InGame;

export namespace SaveLoad
{
    namespace fs = std::filesystem;
    inline GameObject* stageParent{nullptr};
    constexpr auto commonPath = L"scene";
    constexpr auto stagePath = L"scene/stages";
    constexpr auto tilePath = L"scene/tiles";
    constexpr auto doodadPath = L"scene/doodads";
    struct StageInfo
    {
        std::wstring stageGUID;
        double posX, posY, posZ;
        double rotX, rotY, rotZ, rotW;
        unsigned int rowLength; // == height
        unsigned int colLength; // == width
        double arrowYDisplacement;
        unsigned int stageIndex;
    };
    struct TileInfo
    {
        std::wstring tileGUID;
        std::wstring stageGUID;
        double posX, posY, posZ;
        double rotX, rotY, rotZ, rotW;
        unsigned int tileType;
        unsigned int row, col;
        unsigned int laserDirection;
        unsigned int energy;
        unsigned int mechanismIndex;
    };
    struct DoodadInfo
    {
        double posX, posY, posZ;
        double rotX, rotY, rotZ, rotW;
        double sclX, sclY, sclZ;
        std::wstring stageGUID;
        std::wstring staticMesh;
        std::wstring diffuseTexture;
        std::wstring normalTexture;
    };
    template<typename ComponentType>
    std::vector<ComponentType*> GetComponentsFromChildren(IGameObjectParent* parent)
    {
        auto children = parent->GetChildren();
        std::vector<ComponentType*> ret;
        for (auto child : children)
        {
            if (auto comp = child->GetComponent<ComponentType>(); comp != nullptr)
                ret.push_back(comp);
            auto vec = GetComponentsFromChildren<ComponentType>(child);
            for (auto comp : vec)
                ret.push_back(comp);
        }
        return ret;
    }
    inline std::vector<Stage*> GetComponentsFromChildren2(IGameObjectParent* parent)
    {
        auto children = parent->GetChildren();
        std::vector<Stage*> ret;
        for (auto child : children)
        {
            if (auto comp = child->GetComponent<Stage>(); comp != nullptr)
                ret.push_back(comp);
            for (auto comp : GetComponentsFromChildren2(child))
                ret.push_back(comp);
        }
        return ret;
    }
    inline std::vector<Tile*> GetComponentsFromChildren3(IGameObjectParent* parent)
    {
        auto children = parent->GetChildren();
        std::vector<Tile*> ret;
        for (auto child : children)
        {
            if (auto comp = child->GetComponent<Tile>(); comp != nullptr)
                ret.push_back(comp);
            for (auto comp : GetComponentsFromChildren3(child))
                ret.push_back(comp);
        }
        return ret;
    }
    void DeleteFilesInside(std::wstring path)
    {
        for (const auto& entry : fs::directory_iterator(path))
            if (fs::is_regular_file(entry))
            {
                try
                {
                    fs::remove(entry);
                }
                catch (const std::exception& ex)
                {
                }
            }
    }
    yutility::GenericJsonStructHandler<StageInfo> stageJsonHandler{
        std::pair{ L"posX", & StageInfo::posX },
            std::pair{ L"posY", & StageInfo::posY },
            std::pair{ L"posZ", & StageInfo::posZ },
            std::pair{ L"rotX", & StageInfo::rotX },
            std::pair{ L"rotY", & StageInfo::rotY },
            std::pair{ L"rotZ", & StageInfo::rotZ },
            std::pair{ L"rotW", & StageInfo::rotW },
            std::pair{ L"stage index", & StageInfo::stageIndex },
            std::pair{ L"rowLength", & StageInfo::rowLength },
            std::pair{ L"colLength", & StageInfo::colLength },
            std::pair{ L"arrow y displacement", & StageInfo::arrowYDisplacement },
            std::pair{ L"guid", & StageInfo::stageGUID },
    };
    yutility::GenericJsonStructHandler<TileInfo> tileJsonHandler{
        std::pair{ L"posX", & TileInfo::posX },
            std::pair{ L"posY", & TileInfo::posY },
            std::pair{ L"posZ", & TileInfo::posZ },
            std::pair{ L"rotX", & TileInfo::rotX },
            std::pair{ L"rotY", & TileInfo::rotY },
            std::pair{ L"rotZ", & TileInfo::rotZ },
            std::pair{ L"rotW", & TileInfo::rotW },
            std::pair{ L"stage guid", & TileInfo::stageGUID},
            std::pair{ L"tileType", & TileInfo::tileType },
            std::pair{ L"row", & TileInfo::row },
            std::pair{ L"col", & TileInfo::col },
            std::pair{ L"laserDirection", & TileInfo::laserDirection },
            std::pair{ L"energy", & TileInfo::energy },
            std::pair{ L"mechanism index", & TileInfo::mechanismIndex },
    };
    yutility::GenericJsonStructHandler<DoodadInfo> doodadJsonHandler{
        std::pair{ L"posX", & DoodadInfo::posX },
            std::pair{ L"posY", & DoodadInfo::posY },
            std::pair{ L"posZ", & DoodadInfo::posZ },
            std::pair{ L"rotX", & DoodadInfo::rotX },
            std::pair{ L"rotY", & DoodadInfo::rotY },
            std::pair{ L"rotZ", & DoodadInfo::rotZ },
            std::pair{ L"rotW", & DoodadInfo::rotW },
            std::pair{ L"sclX", & DoodadInfo::sclX },
            std::pair{ L"sclY", & DoodadInfo::sclY },
            std::pair{ L"sclZ", & DoodadInfo::sclZ },
            std::pair{ L"stage guid", & DoodadInfo::stageGUID},
            std::pair{ L"static mesh id", & DoodadInfo::staticMesh},
            std::pair{ L"diffuse texture", & DoodadInfo::diffuseTexture},
            std::pair{ L"normal texture", & DoodadInfo::normalTexture},
    };

    void SaveScene()
    {
        std::filesystem::create_directory(commonPath);
        std::filesystem::create_directory(stagePath);
        std::filesystem::create_directory(tilePath);
        std::filesystem::create_directory(doodadPath);
        // delete existing files
        DeleteFilesInside(stagePath);
        DeleteFilesInside(tilePath);
        DeleteFilesInside(doodadPath);

        std::vector<Stage*> stages = GetComponentsFromChildren<Stage>(Scene::getCurrentScene());
        std::vector<Tile*> tiles = GetComponentsFromChildren3(Scene::getCurrentScene());
        std::vector<StaticMeshWrapper*> doodads = GetComponentsFromChildren<StaticMeshWrapper>(Scene::getCurrentScene());
        //std::vector<Tile*> tiles2 = GetComponentsFromChildren<Tile>(Scene::getCurrentScene());
        for (auto each : stages)
        {
            StageInfo eachInfo
            {
                .stageGUID = each->GetGUIDWStr(),
                .posX = each->GetTransform()->GetWorldPosition().x,
                .posY = each->GetTransform()->GetWorldPosition().y,
                .posZ = each->GetTransform()->GetWorldPosition().z,
                .rotX = each->GetTransform()->GetWorldRotation().x,
                .rotY = each->GetTransform()->GetWorldRotation().y,
                .rotZ = each->GetTransform()->GetWorldRotation().z,
                .rotW = each->GetTransform()->GetWorldRotation().w,
                .rowLength = static_cast<unsigned int> (each->m_goalHeightSize),
                .colLength = static_cast<unsigned int>(each->m_goalWidthSize),
                .arrowYDisplacement = each->arrowInterval,
                .stageIndex = static_cast<unsigned int>(each->m_StageIndex),
            };
            std::wofstream stageFile{stagePath + std::wstring(L"/") + each->GetGUIDWStr(), std::ios::trunc};
            stageFile << stageJsonHandler.SerializeData(eachInfo);
            stageFile.close();
        }
        for (auto each : tiles)
        {
            TileInfo eachInfo
            {
                .tileGUID = each->GetGUIDWStr(),
                .stageGUID = each->_stage->GetGUIDWStr(),
                .posX = each->GetTransform()->GetWorldPosition().x,
                .posY = each->GetTransform()->GetWorldPosition().y,
                .posZ = each->GetTransform()->GetWorldPosition().z,
                .rotX = each->GetTransform()->GetWorldRotation().x,
                .rotY = each->GetTransform()->GetWorldRotation().y,
                .rotZ = each->GetTransform()->GetWorldRotation().z,
                .rotW = each->GetTransform()->GetWorldRotation().w,
                .tileType = static_cast<unsigned int>(each->tileType),
                .row = static_cast<unsigned int>(each->_row),
                .col = static_cast<unsigned int>(each->_col),
                .laserDirection = static_cast<unsigned int>(static_cast<unsigned int>(each->LaserDirection)),
                .energy = static_cast<unsigned int>(each->energy),
                .mechanismIndex = static_cast<unsigned int>(each->mechanismIndex),
            };
            std::wofstream tileFile{tilePath + std::wstring(L"/") + each->GetGUIDWStr(), std::ios::trunc};
            tileFile << tileJsonHandler.SerializeData(eachInfo);
            tileFile.close();
        }
        for (auto each : doodads)
        {
            auto parent = each->GetGameObject()->GetParentGameObject();
            if (parent == nullptr)
                continue;
            auto comp = parent->GetComponent<Stage>();
            if (comp == nullptr)
                continue;

            DoodadInfo eachInfo
            {
                .posX = each->GetTransform()->position.x,
                .posY = each->GetTransform()->position.y,
                .posZ = each->GetTransform()->position.z,
                .rotX = each->GetTransform()->rotation.x,
                .rotY = each->GetTransform()->rotation.y,
                .rotZ = each->GetTransform()->rotation.z,
                .rotW = each->GetTransform()->rotation.w,
                .sclX = each->GetTransform()->scale.x,
                .sclY = each->GetTransform()->scale.y,
                .sclZ = each->GetTransform()->scale.z,
                .stageGUID = comp->GetGUIDWStr(),
                .staticMesh = yutility::GetWString(each->meshFilePath),
                .diffuseTexture = yutility::GetWString(each->diffuseFilePath),
                .normalTexture = yutility::GetWString(each->normalFilePath),
            };
            std::wofstream doodadFile{doodadPath + std::wstring(L"/") + each->GetGUIDWStr(), std::ios::trunc};
            doodadFile << doodadJsonHandler.SerializeData(eachInfo);
            doodadFile.close();
        }
    }
    void LoadScene()
    {
        // 스테이지 정보
        vector<StageInfo> stageInfos;
        // 타일 정보
        vector<TileInfo> tileInfos;
        // 장식물 정보
        vector<DoodadInfo> doodadInfos;
        std::filesystem::create_directory(doodadPath);
        // Stage를 지우면 Tile도 다 사라짐
        std::vector<Stage*> stages = GetComponentsFromChildren<Stage>(Scene::getCurrentScene());
        for (auto each : stages)
            Scene::getCurrentScene()->DestroyGameObject(each->GetGameObject());

        for (const auto& entry : fs::directory_iterator(stagePath))
        {
            if (fs::is_regular_file(entry))
            {
                std::wifstream file(entry.path());
                file.seekg(0, std::ios::end);
                std::streamsize fileSize = file.tellg();
                file.seekg(0, std::ios::beg);

                std::wstring fileContents;
                fileContents.reserve(fileSize);

                wchar_t ch;
                while (file.get(ch)) {
                    fileContents.push_back(ch);
                }
                file.close();
                stageInfos.push_back(stageJsonHandler.ReadData(fileContents.c_str()));
            }
        }
        for (const auto& entry : fs::directory_iterator(tilePath))
        {
            if (fs::is_regular_file(entry))
            {
                std::wifstream file(entry.path());
                file.seekg(0, std::ios::end);
                std::streamsize fileSize = file.tellg();
                file.seekg(0, std::ios::beg);

                std::wstring fileContents;
                fileContents.reserve(fileSize);

                wchar_t ch;
                while (file.get(ch)) {
                    fileContents.push_back(ch);
                }
                file.close();
                tileInfos.push_back(tileJsonHandler.ReadData(fileContents.c_str()));
            }
        }
        for (const auto& entry : fs::directory_iterator(doodadPath))
        {
            if (fs::is_regular_file(entry))
            {
                std::wifstream file(entry.path());
                file.seekg(0, std::ios::end);
                std::streamsize fileSize = file.tellg();
                file.seekg(0, std::ios::beg);

                std::wstring fileContents;
                fileContents.reserve(fileSize);

                wchar_t ch;
                while (file.get(ch)) {
                    fileContents.push_back(ch);
                }
                file.close();
                doodadInfos.push_back(doodadJsonHandler.ReadData(fileContents.c_str()));
            }
        }
        for (auto& info : stageInfos)
        {
            auto guid = info.stageGUID;
            auto stage = Scene::getCurrentScene()->AddGameObject()->AddComponent<Stage>(static_cast<WCHAR*>(&info.stageGUID[0]));
            stage->GetGameObject()->SetParent(stageParent);
            stage->arrowInterval = info.arrowYDisplacement;
            stage->arrowInterval = info.arrowYDisplacement;
            stage->GetTransform()->SetWorldPosition(Vector3d{ info.posX,info.posY,info.posZ });
            stage->GetTransform()->SetWorldRotation(Quaternion{ info.rotW, info.rotX,info.rotY,info.rotZ });
            stage->m_goalHeightSize = info.rowLength;
            stage->m_goalWidthSize = info.colLength;
            stage->m_StageIndex = info.stageIndex;
            stage->Update();
            for (int i = 0; i < info.rowLength; i++)
            {
                stage->tiles.push_back(vector<Tile*>());
                for (int j = 0; j < info.colLength; j++)
                    stage->tiles[i].push_back(nullptr);
            }
        }
        for (const auto& info : tileInfos)
        {
            yunutyEngine::YunutyCycle::SingleInstance().ReserveActionAfterUpdate(
                [=]() {
                    auto guid = info.stageGUID;
                    auto stage = Component::LoadReferenceByGUID<Stage>(info.stageGUID.c_str());
                    auto tile = stage->tiles[info.row][info.col] = stage->AddTile(info.row, info.col);
                    tile->GetTransform()->SetWorldPosition(Vector3d{ info.posX,info.posY,info.posZ });
                    //tile->GetTransform()->SetWorldRotation(Quaternion{ info.rotW, info.rotX,info.rotY,info.rotZ });
                    tile->tileType = static_cast<Tile::TileType>(info.tileType);
                    tile->energy = info.energy;
                    tile->mechanismIndex = info.mechanismIndex;
                    tile->LaserDirection = static_cast<Tile::Direction>(info.laserDirection);
                    tile->ApplyChanges();
                }
            );
        }
        for (const auto& info : doodadInfos)
        {
            yunutyEngine::YunutyCycle::SingleInstance().ReserveActionAfterUpdate(
                [=]() {
                    auto guid = info.stageGUID;
                    auto stage = Component::LoadReferenceByGUID<Stage>(info.stageGUID.c_str());
                    auto doodadObj = stage->GetGameObject()->AddGameObject();
                    auto doodad = doodadObj->AddComponent<StaticMeshWrapper>();
                    doodad->GetTransform()->position = Vector3d{ info.posX,info.posY,info.posZ };
                    doodad->GetTransform()->rotation = Quaternion{ info.rotW, info.rotX,info.rotY,info.rotZ };
                    doodad->GetTransform()->scale = Vector3d{ info.sclX,info.sclY,info.sclZ };
                    doodad->meshFilePath = yutility::GetString(info.staticMesh);
                    doodad->diffuseFilePath = yutility::GetString(info.diffuseTexture);
                    doodad->normalFilePath = yutility::GetString(info.normalTexture);
                    doodad->ApplySettings();
                }
            );
        }
    }


};
