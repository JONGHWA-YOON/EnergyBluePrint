#include "YunutyEngine.h"
#include "ConsoleCamera.h"
#include "Transform.h"
#include "MatrixImage.h"
#include "GameObject.h"

using namespace YunutyEngine;
using namespace YunutyEngine::Console;

void YunutyEngine::Console::ConsoleCamera::Render()
{
    if (spaceWidth * spaceHeight != 0)
        MoveWindow(hwnd, 0, 0, spaceWidth * widthPixelRatio, spaceHeight * heightPixelRatio, false);

    for (int i = 0; i < spaceHeight; i++)
        for (int j = 0; j < spaceWidth; j++)
            screenSpaceMatrix[i][j] = CharNode();

    for (auto matrixImage : MatrixImage::GetMatrixImages())
    {
        if (!matrixImage->GetGameObject()->GetActive())
            continue;
        auto rowcolStart = GetColRow(matrixImage->getUpperLeftXY());
        auto matrix = matrixImage->GetImageMatrix();
        for (auto row = 0; row < matrixImage->GetHeight(); row++)
            for (auto col = 0; col < matrixImage->GetWidth(); col++)
            {
                int screenRow = rowcolStart.y + row;
                int screenCol = rowcolStart.x + col;
                int index = matrixImage->GetGameObject()->GetSceneIndex();

                if (screenRow < 0 || screenRow >= spaceHeight || screenCol < 0 || screenCol >= spaceWidth)
                    continue;
                if (matrix[row][col] == '\0')
                    continue;
                if (matrix[row][col] == '\n')
                    break;

                if (index >= screenSpaceMatrix[screenRow][screenCol].currentPriority)
                    screenSpaceMatrix[screenRow][screenCol] = CharNode(index, matrix[row][col]);
            }
    }
    COORD zero;
    zero.X = 0;
    zero.Y = 0;
    SetConsoleCursorPosition(hConsole, zero);
    string screen;
    if (!screenSpaceMatrix.empty())
    {
        for (int i = 0; i < spaceHeight; i++)
        {
            for (int j = 0; j < spaceWidth; j++)
                screen.push_back(screenSpaceMatrix[i][j].character);
            /*for (int j = 0; j < spaceWidth; j++)
                putchar(screenSpaceMatrix[i][j].character);*/

            screen.push_back('\n');
            //putchar('\n');
        }
    }
    cout << screen;
}

int YunutyEngine::Console::ConsoleCamera::GetSpaceWidth()
{
    return spaceWidth;
}

void YunutyEngine::Console::ConsoleCamera::SetSpaceWidth(int width)
{
    spaceWidth = width;
    if (spaceWidth * spaceHeight != 0)
        screenSpaceMatrix = vector<vector<CharNode>>(spaceHeight, vector<CharNode>(spaceWidth));
}

int YunutyEngine::Console::ConsoleCamera::GetSpaceHeight()
{
    return spaceHeight;
}

void YunutyEngine::Console::ConsoleCamera::SetSpaceHeight(int height)
{
    spaceHeight = height;
    if (spaceWidth * spaceHeight != 0)
        screenSpaceMatrix = vector<vector<CharNode>>(spaceHeight, vector<CharNode>(spaceWidth));
}

Vector2<int> YunutyEngine::Console::ConsoleCamera::GetUpperLeftXY()
{
    return Vector2<int>(GetTransform()->GetWorldPosition().x - spaceWidth / 2, GetTransform()->GetWorldPosition().y + spaceHeight / 2);
}
Vector2<int> YunutyEngine::Console::ConsoleCamera::GetColRow(int x, int y)
{
    int row = -(y - GetUpperLeftXY().y);
    int col = x - GetUpperLeftXY().x;
    return Vector2<int>(col, row);
}
Vector2<int> YunutyEngine::Console::ConsoleCamera::GetColRow(Vector2<int> xy)
{
    return GetColRow(xy.x, xy.y);
}
