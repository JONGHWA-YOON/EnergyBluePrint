#pragma once
#include <stdexcept>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    template<typename JsonStructType>
    class cache
    {
    private:
        JsonStructType data;
        bool dirty = true;
    public:
        cache() :data(JsonStructType()) {}
        cache(const JsonStructType& data) :data(data), dirty(false) {  }
        operator JsonStructType() const
        {
#if _DEBUG
            if (dirty)
                throw std::runtime_error(std::string("trying to read cahced data while it's dirty!"));
#endif
            return data;
        }
        JsonStructType& operator=(const JsonStructType& data) { dirty = false; return this->data = data; }
        JsonStructType& operator=(const cache& cachedData) { dirty = false; return this->data = cachedData->data; }
        void SetDirty() { dirty = true; }
        bool IsDirty() { return dirty; }
    };
}
