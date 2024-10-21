module;
#include "YunutyEngine.h"
#include "Timer.h"
module htkg.InGame.Robot;
import htkg.InGame.Tile;

void Robot::TimerUpdateCallback(float normT)
{
    switch (state)
    {
    case Robot::State::Ready:
        break;
    case Robot::State::Walking:
        GetTransform()->SetWorldPosition
        (Vector3d::Lerp(moveCurrent->GetTransform()->GetWorldPosition(), moveNext->GetTransform()->GetWorldPosition(), 1 - (1 - normT) * (1 - normT)));
        break;
    case Robot::State::Draining:
        break;
    case Robot::State::BeingDrained:
        break;
    default:
        break;
    }
}
void Robot::TimerExpirationCallback()
{
    switch (state)
    {
    case Robot::State::Ready:
        break;
    case Robot::State::Walking:
        state = Robot::State::Ready;
        break;
    case Robot::State::Draining:
        break;
    case Robot::State::BeingDrained:
        break;
    default:
        break;
    }
}
void Robot::PreProceed()
{
    switch (state)
    {
    case Robot::State::Ready:
        break;
    case Robot::State::Walking:
        break;
    case Robot::State::Draining:
        break;
    case Robot::State::BeingDrained:
        break;
    default:
        break;
    }
}
void Robot::Proceed()
{
    switch (state)
    {
    case Robot::State::Ready:
        if (energy == 0 && currentTile->tileType != Tile::TileType::Destination)
        {
            state = Robot::State::Shutdown;
            break;
        }
        energy--;

        if (!moveQueue.empty())
            while (moveQueue.front() == currentTile)
                moveQueue.pop_front();

        if (!moveQueue.empty())
        {
            moveCurrent = currentTile;
            moveNext = moveQueue.front();
            currentTile = moveNext;
            moveQueue.pop_front();
            state = State::Walking;
        }
        else
        {
        }
        return;


        break;
    case Robot::State::Walking:
        break;
    case Robot::State::Draining:
        break;
    case Robot::State::BeingDrained:
        break;
    default:
        break;
    }
    actionTimer.Start();
}
void Robot::PostProceed()
{
    if (currentTile->tileType == Tile::TileType::Destination)
        state = State::Victorious;

    switch (state)
    {
    case Robot::State::Ready:
        break;
    case Robot::State::Walking:
        break;
    case Robot::State::Draining:
        break;
    case Robot::State::BeingDrained:
        break;
    default:
        break;
    }
}

