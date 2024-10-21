#include "YunutyEngine.h"
#include "MatrixCustomImage.h"
#include <string>
#include <sstream>
using namespace YunutyEngine;
using namespace YunutyEngine::Console;
using namespace std;

void YunutyEngine::Console::MatrixCustomImage::SetMatrixString(const string& imageString)
{
    SetMatrix(imageString);
}
void YunutyEngine::Console::MatrixCustomImage::SetMatrixStrings(const vector<string>& imageStrings)
{
    SetMatrix(imageStrings);
}
