module;
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "YunutyEngine.h"
#include "Timer.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <functional>
#include "ButtonLogicManager.h"
module htkg.InGame.Stage;

import htkg.InGame.Battery;
import htkg.InGame.Button;
import htkg.InGame.Laser;
import htkg.InGame.Robot;
import htkg.InGame.Tile;
import htkg.ResourcesID;
import htkg.SaveLoad;
import htkg.ButtonLogicManager;
import ui.DebugPanel;

void Stage::LoadNextStage(int currentStageIndex)
{
    std::map<int, Stage*, std::less<>> stages;
    for (auto stage : SaveLoad::GetComponentsFromChildren<Stage>(Scene::getCurrentScene()))
    {
        stages[stage->m_StageIndex] = stage;
        stage->GetGameObject()->SetSelfActive(false);
    }
    auto itr = stages.upper_bound(currentStageIndex);
    Stage::currentStage = itr == stages.end() ? nullptr : itr->second;

    if (Stage::currentStage == nullptr)
        ButtonLogicManager::instance->GetLogicMap().at(ButtonLogicManager::eButtonType::MenuQuit)();
    else
        Stage::currentStage->ResetStage();
}
void Stage::Proceed()
{
    bool stillAlive = false;
    for (auto robot : robots)
        stillAlive |= robot->state != Robot::State::Shutdown;

    if (stillAlive == false)
        ButtonLogicManager::instance->StageFailLogic();

    for (auto robot : robots)
        if (robot->state == Robot::State::Victorious)
            ButtonLogicManager::instance->StageClearLogic();

    for (auto robot : robots)
        robot->PreProceed();
    for (auto robot : robots)
        robot->Proceed();
    for (auto robot : robots)
        robot->PostProceed();

    if (!isPlaying)
        playingTimer.isActive = false;
}
