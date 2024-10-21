module;
#include <cstdlib>
#include "ButtonLogicManager.h"
#include "YunutyEngine.h"
#include "DebugManager.h"
module htkg.ButtonLogicManager;
import htkg.InGame.Stage;

ButtonLogicManager* ButtonLogicManager::instance{nullptr};
ButtonLogicManager::ButtonLogicManager()
{
    instance = this;
    SetLogicMap();
}

ButtonLogicManager::~ButtonLogicManager()
{

}

void ButtonLogicManager::UiCanvasLogic(std::string activeCanvas)
{
    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
        if (obj->getName() == "Canvas")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == activeCanvas)
                {
                    child->SetSelfActive(true);
                }
                else
                {
                    child->SetSelfActive(false);
                }
            }
        }
        else if (obj->getName() != "Editor Camera")
        {
            obj->SetSelfActive(false);
        }
    }
}

void ButtonLogicManager::OpenMainMenu()
{
    UiCanvasLogic("Main Menu Canvas");
}

// Main Menu
void ButtonLogicManager::GameStartButton()
{
    UiCanvasLogic("Stage Select Canvas");
}

void ButtonLogicManager::CreditButton()
{
    UiCanvasLogic("Credit Canvas");
}

void ButtonLogicManager::GameQuitButton()
{
    DebugManager::GetInstance()->m_isPlayMode = false;
    Stage::LoadNextStage(0);
    UiCanvasLogic("Title Canvas");
}

// Stage Select
void ButtonLogicManager::PlayButton()
{
    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
        if (obj->getName() == "InGame")
        {
            obj->SetSelfActive(true);
        }

        else if (obj->getName() == "Canvas")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == "Ingame UI Canvas")
                {
                    child->SetSelfActive(true);
                }
                else
                {
                    child->SetSelfActive(false);
                }
            }
        }

        else if (obj->getName() != "Editor Camera")
        {
            obj->SetSelfActive(false);
        }
    }
    Stage::LoadNextStage(-1);
}

void ButtonLogicManager::StageButton()
{
    // 스테이지 선택? UI
}

void ButtonLogicManager::BackButton()
{
    OpenMainMenu();
}

// Ingame
void ButtonLogicManager::RunButton()
{
    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
        if (obj->getName() == "Ingame")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == "Play Button")
                {
                    child->SetSelfActive(false);
                }
                break;
            }
        }
    }
    Stage::Run();

    // 런이 끝났을때는 다시 true로 활성화를 바꿔줘야함 / 아니면 그냥 계속 떠있게 
}

void ButtonLogicManager::MenuButton()
{
    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
        if (obj->getName() == "Canvas")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == "Ingame Menu Canvas")
                {
                    child->SetSelfActive(true);
                }
                else
                {
                    child->SetSelfActive(false);
                }
            }
            break;
        }
    }
}

// Ingame Menu
void ButtonLogicManager::ContinueButton()
{
    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
        if (obj->getName() == "Canvas")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == "Ingame Menu Canvas")
                {
                    child->SetSelfActive(false);
                }
                else if (child->getName() == "Ingame UI Canvas")
                {
                    child->SetSelfActive(true);
                }
            }
            break;
        }
    }
}

void ButtonLogicManager::RestartButton()
{
    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
		if (obj->getName() == "InGame")
		{
			obj->SetSelfActive(true);
		}

        else if (obj->getName() == "Canvas")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == "Ingame UI Canvas")
                {
                    child->SetSelfActive(true);
                }
                else
                {
                    child->SetSelfActive(false);
                }
            }
        }
    }

    // + 리스타트시 인게임 초기화할 것들 초기화
    Stage::Reset();
}


void ButtonLogicManager::MenuQuitButton()
{
    OpenMainMenu();
}

// Stage Clear
void ButtonLogicManager::NextStageButton()
{
    Stage::LoadNextStage(Stage::currentStage ? Stage::currentStage->m_StageIndex : -1);

    for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
    {
        if (obj->getName() == "InGame")
        {
            obj->SetSelfActive(true);
        }

        else if (obj->getName() == "Canvas")
        {
            for (auto child : obj->GetChildren())
            {
                if (child->getName() == "Ingame UI Canvas")
                {
                    child->SetSelfActive(true);
                }
                else
                {
                    child->SetSelfActive(false);
                }
            }
        }
    }
}

void ButtonLogicManager::StageClearLogic()
{
    // 스테이지 클리어 조건 로직 넣어주세요

    // 스테이지 클리어시 UI 그리기
    DrawStageClearCanvas();
}

void ButtonLogicManager::StageFailLogic()
{
	for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
	{
		if (obj->getName() == "InGame")
		{
			obj->SetSelfActive(false);
		}

		else if (obj->getName() == "Canvas")
		{
			for (auto child : obj->GetChildren())
			{
				if (child->getName() == "Stage Fail Canvas")
				{
					child->SetSelfActive(true);
				}
				else if (child->getName() == "Ingame UI Canvas")
				{
					child->SetSelfActive(false);
				}
			}
		}
	}
}

void ButtonLogicManager::DrawStageClearCanvas()
{
	for (auto obj : yunutyEngine::Scene::getCurrentScene()->GetChildren())
	{
		if (obj->getName() == "InGame")
		{
			obj->SetSelfActive(false);
		}

		else if (obj->getName() == "Canvas")
		{
			for (auto child : obj->GetChildren())
			{
				if (child->getName() == "Stage Clear Canvas")
				{
					child->SetSelfActive(true);
				}
				else if (child->getName() == "Ingame UI Canvas")
				{
					child->SetSelfActive(false);
				}
			}
		}
	}
}

const std::unordered_map<ButtonLogicManager::eButtonType, std::function<void()>>& ButtonLogicManager::GetLogicMap()
{
    return m_buttonLogic;
}

void ButtonLogicManager::SetLogicMap()
{
    m_buttonLogic.insert(std::make_pair(eButtonType::None, nullptr));

    m_buttonLogic.insert(std::make_pair(eButtonType::GameStart, [this]() { GameStartButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::Credit, [this]() { CreditButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::GameQuit, [this]() { GameQuitButton(); }));

    m_buttonLogic.insert(std::make_pair(eButtonType::Play, [this]() { PlayButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::Stage, [this]() { StageButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::Back, [this]() { BackButton(); }));

    m_buttonLogic.insert(std::make_pair(eButtonType::Run, [this]() { RunButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::Menu, [this]() { MenuButton(); }));

    m_buttonLogic.insert(std::make_pair(eButtonType::Continue, [this]() { ContinueButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::Restart, [this]() { RestartButton(); }));
    m_buttonLogic.insert(std::make_pair(eButtonType::MenuQuit, [this]() { MenuQuitButton(); }));

    m_buttonLogic.insert(std::make_pair(eButtonType::NextStage, [this]() { NextStageButton(); }));
	m_buttonLogic.insert(std::make_pair(eButtonType::StageClear, [this]() { DrawStageClearCanvas(); }));

}
