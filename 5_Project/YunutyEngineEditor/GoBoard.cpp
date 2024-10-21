module;
#include <iostream>
#include "YunutyEngine.h"
module htkg.components.GoBoard;
import ui.DebugPanel;

int GoBoard::GetHeight()
{
    return m_goalHeightSize;
}

int GoBoard::GetWidth()
{
    return m_goalWidthSize;
}

void GoBoard::SetHeight(int size)
{
    m_goalHeightSize = size;
}

void GoBoard::SetWidth(int size)
{
    m_goalWidthSize = size;
}

void GoBoard::SetBoard(yunutyEngine::GameObject* obj)
{
    if (m_InitFlag == true)
    {
        parentObject = Scene::getCurrentScene()->AddGameObject();
        parentObject->setName("GoBoard");
        parentObject->SetParent(obj);

        m_InitFlag = false;
    }

    if (m_goBoard.size() > m_goalHeightSize)
    {
        for (int i = m_goalHeightSize; i < m_goBoard.size(); i++)
        {
            for (int j = 0; j < m_goBoard[i].size(); j++)
            {
                Scene::getCurrentScene()->DestroyGameObject(m_goBoard[i][j]);
            }
        }
    }

    if (m_goBoard.size() > 0)
    {
        if (m_goBoard[0].size() > m_goalWidthSize)
        {
            for (int i = m_goalWidthSize; i < m_goBoard[0].size(); i++)
            {
                for (int j = 0; j < m_goBoard.size(); j++)
                {
                    Scene::getCurrentScene()->DestroyGameObject(m_goBoard[j][i]);
                }
            }
        }
    }
    if (m_prevBoardSizeH != m_goalHeightSize || m_prevBoardSizeW != m_goalWidthSize)
    {
        m_goBoard.resize(m_goalHeightSize);
        for (int i = 0; i < m_goBoard.size(); i++)
        {
            m_goBoard[i].resize(m_goalWidthSize);
        }


        for (int i = 0; i < m_goalHeightSize; i++)
        {
            for (int j = 0; j < m_goalWidthSize; j++)
            {
                if (m_goBoard[i][j] == nullptr)
                {
                    m_goBoard[i][j] = Scene::getCurrentScene()->AddGameObject();
                    m_goBoard[i][j]->SetParent(parentObject);
                    m_goBoard[i][j]->GetTransform()->position.x = (m_interval + 1) * i;
                    m_goBoard[i][j]->GetTransform()->position.z = (m_interval + 1) * j;
                    std::string boardIndex = std::to_string(i + 1) + "," + std::to_string(j + 1);
                    m_goBoard[i][j]->setName("board " + boardIndex);
                    auto staticMeshComp = m_goBoard[i][j]->AddComponent<yunutyEngine::graphics::StaticMesh>();
                    staticMeshComp->GetGI().LoadMesh("FBXFile/static_box.fbx");
                    staticMeshComp->GetGI().LoadDiffuseMap("Textures/000000002405_reverse.dds");
                    auto eventHandler = m_goBoard[i][j]->AddComponent<yunutyEngine::graphics::EventHandler>();
                    eventHandler->SetClickCallback([=]() {
                        DebugPanel::LogMessage((m_goBoard[i][j]->getName() + " clicked").c_str());
                        });
                    eventHandler->SetHoverCallback([=]() {
                        DebugPanel::LogMessage((m_goBoard[i][j]->getName() + " hovered").c_str());
                        });
                }
            }
        }
        m_prevBoardSizeH = m_goalHeightSize;
        m_prevBoardSizeW = m_goalWidthSize;
    }

    if (m_prevInterval != m_interval)
    {
        SetInterval();
        m_prevInterval = m_interval;
    }
}

void GoBoard::SetInterval()
{
    for (int i = 0; i < m_goalHeightSize; i++)
    {
        for (int j = 0; j < m_goalWidthSize; j++)
        {
            if (m_goBoard[i][j] != nullptr)
            {
                m_goBoard[i][j]->GetTransform()->position.x = (m_interval + 1) * i;
                m_goBoard[i][j]->GetTransform()->position.z = (m_interval + 1) * j;
            }
        }
    }
}