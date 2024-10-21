#pragma once
#include "Graphic.h"
#include <vector>
#include <unordered_set>
#include "Vector2.h"


using namespace std;
namespace YunutyEngine
{
    namespace Console
    {
        class MatrixImage :
            public Graphic
        {
        protected:
            vector<vector<char>> imageMatrix;
            static unordered_set<MatrixImage*> matrixImages;
            void SetMatrix(const string& imageString);
            void SetMatrix(const vector<string>& imageStrings);
            int width = 0;
            int height = 0;
        public:
            const vector<vector<char>>& GetImageMatrix();
            MatrixImage();
            virtual ~MatrixImage();
            static const unordered_set<MatrixImage*>& GetMatrixImages();
            int GetWidth();
            int GetHeight();
            Vector2<int> getUpperLeftXY();
        };
    }
}
