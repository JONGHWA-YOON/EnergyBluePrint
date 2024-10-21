#pragma once

template<typename T>
class LazyObject
{
public:
    T& Get()
    {
        if (!instance)
            instance = new T();
        return *instance;
    }
    void Release()
    {
        if (!instance)
            return;

        delete instance;
        instance = nullptr;
    }
private:
    T* instance = nullptr;
};
