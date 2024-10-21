#pragma once
#include <unordered_set>
#include <unordered_map>
#include "Input.h"

namespace YunutyEngine
{
    namespace Console
    {
        class ConsoleInput :
            public Input
        {
        private:
            unordered_set<KeyCode> keyDownSet;
            unordered_set<KeyCode> keyPushedSet;
            unordered_map<unsigned int, KeyCode> rawKeyEnumMap;
        protected:
            virtual void Update();
            virtual bool m_isKeyDown(KeyCode keyCode);
            virtual bool m_isKeyPushed(KeyCode keyCode);
        public:

            ConsoleInput();
        };
    }
}
