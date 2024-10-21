#pragma once
// 싱글톤 클래스를 자식 클래스타입을 템플릿 파라미터로 받는 템플릿 클래스로 구현한다.

namespace yunutyEngine
{
    template<typename T>
    class SingletonClass
    {
    public:
        static T& SingleInstance();
    private:
        static T* instance;
    };
}
template<typename T>
T& yunutyEngine::SingletonClass<T>::SingleInstance()
{
    if (instance == nullptr)
        instance = new T();
    return *instance;
}
template<typename T>
T* yunutyEngine::SingletonClass<T>::instance = nullptr;
