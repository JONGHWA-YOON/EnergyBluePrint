#pragma once
#include <vector>
#include "Camera.h"
#include "Vector2.h"
#include <Windows.h>

typedef unsigned short int;
using namespace std;
namespace YunutyEngine
{
    namespace Console
    {
        class ConsoleCamera :
            public Camera
        {
        private:
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            HWND hwnd = GetConsoleWindow();
            const float widthPixelRatio=8.2;
            const float heightPixelRatio=17.5;
            class CharNode
            {
            public:
                int currentPriority = 0;
                char character = ' ';
                CharNode(int currentPriority, char character) :currentPriority(currentPriority), character(character) {};
                CharNode() : CharNode(0, ' ') {};
            };
            vector<vector<CharNode>> screenSpaceMatrix;
            int spaceWidth=0;
            int spaceHeight=0;
            Vector2<int> GetUpperLeftXY();
            Vector2<int> GetColRow(int x, int y);
            Vector2<int> GetColRow(Vector2<int> xy);
        protected:
            void Render() override;
        public:
            int GetSpaceWidth();
            void SetSpaceWidth(int width);
            int GetSpaceHeight();
            void SetSpaceHeight(int height);
        };
    }
}
