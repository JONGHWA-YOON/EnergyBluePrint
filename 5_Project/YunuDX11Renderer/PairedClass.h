#pragma once
#include <concepts>

// Paired클래스는 생성되면서 다른 객체를 함께 생성하고, Release 될 때 그 객체를 소멸시킵니다.
template<typename T>
class PairedClass
{
public:
    template<typename... Args> requires std::is_constructible_v<T, Args...>
    PairedClass(Args... args) : pairedObject{ args... } {  }
protected:
    T pairedObject;
};
