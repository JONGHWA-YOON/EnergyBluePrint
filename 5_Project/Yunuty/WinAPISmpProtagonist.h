#pragma once
#include "Component.h"

namespace YunutyEngine
{
    namespace WinAPI
    {
        namespace Sample
        {
            class WinAPISmpProtagonist : public Component
            {
            protected:
                virtual void Update() override;
            };
        }
    }
}
