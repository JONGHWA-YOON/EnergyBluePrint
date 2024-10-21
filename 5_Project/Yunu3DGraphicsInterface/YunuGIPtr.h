#pragma once
#include <concepts>
//#include "IUnknown.h"

namespace yunuGI
{
    class IUnknown;
    template<typename T> requires std::is_base_of_v<IUnknown, T>
    class GIPtr
    {
    public:
        T* Get() { return instance; }
        ~GIPtr() { Release(); }
        GIPtr& operator=(T* instance)
        {
            Release();
            this->instance = instance;
            return *this;
        }
        T* operator->() { return instance; }
        void Release() { yunuGI::Global::ReleaseYunuGI(instance); instance = nullptr; }
    private:
        T* instance{ nullptr };
    };
}
