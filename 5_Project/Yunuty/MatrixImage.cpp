#include "YunutyEngine.h"
#include "MatrixImage.h"
#include <string>
#include <sstream>
#include "Transform.h"

using namespace YunutyEngine;
using namespace YunutyEngine::Console;
using namespace std;

unordered_set<MatrixImage*> MatrixImage::matrixImages;
const vector<vector<char>>& YunutyEngine::Console::MatrixImage::GetImageMatrix()
{
    return this->imageMatrix;
}
MatrixImage::MatrixImage()
{
    matrixImages.insert(this);
}
MatrixImage::~MatrixImage()
{
    matrixImages.erase(this);
}
int MatrixImage::GetWidth()
{
    return width;
}
int MatrixImage::GetHeight()
{
    return height;
}
void YunutyEngine::Console::MatrixImage::SetMatrix(const string& imageString)
{
    stringstream imgStream(imageString);
    string segment;
    vector<string> segments;
    while (getline(imgStream, segment, '\n'))
    {
        segments.push_back(segment);
    }
    SetMatrix(segments);
}
//template<class StringContainer>
void YunutyEngine::Console::MatrixImage::SetMatrix(const vector<string>& imageStrings)
{
    width = 0;
    height = 0;
    height = imageStrings.size();
    for (int i=0;i< imageStrings.size();i++)
        if (imageStrings[i].length() > width)
            width = imageStrings[i].length();

    imageMatrix = vector<vector<char>>(height, vector<char>(width));

    for (int i = 0; i < imageStrings.size(); i++)
    {
        if (imageStrings[i].length() > width)
            width = imageStrings[i].length();

        for (int j = 0; j < imageStrings[i].length(); j++)
            imageMatrix[i][j] = imageStrings[i][j];
    }
}
Vector2<int> YunutyEngine::Console::MatrixImage::getUpperLeftXY()
{
    auto temp=Vector2<int>(GetTransform()->GetWorldPosition().x - width / 2, GetTransform()->GetWorldPosition().y + height / 2);

    return Vector2<int>(GetTransform()->GetWorldPosition().x - width / 2, GetTransform()->GetWorldPosition().y + height / 2);
}
const unordered_set<MatrixImage*>& YunutyEngine::Console::MatrixImage::GetMatrixImages()
{
    return matrixImages;
}
