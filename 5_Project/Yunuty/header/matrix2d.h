#pragma once
#include <vector>

using namespace std;

namespace YunutyEngine
{
    namespace Console
    {
        template<typename T>
        class matrix2d : public vector<vector<T>>
        {
        public:
            matrix2d(int width, int height) :vector<vector<T>>(height, vector<T>(width)) {}
            matrix2d(int width, int height, T defaltValue) :vector<vector<T>>(height, vector<T>(width, defaltValue)) {}
        };
    }
}
