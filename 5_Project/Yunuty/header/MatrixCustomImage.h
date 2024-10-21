#pragma once
#include "MatrixImage.h"
namespace YunutyEngine
{
    namespace Console
    {
        class MatrixCustomImage :
            public MatrixImage
        {
        public:
            void SetMatrixString(const string& imageString);
            void SetMatrixStrings(const vector<string>& imageStrings);
        };

    }
}
