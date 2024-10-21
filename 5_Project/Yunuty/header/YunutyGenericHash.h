#pragma once

namespace yunutyEngine
{
    // yutility 함수 정의, 혹은 functor만 들어간다.
    namespace yutility
    {
        template<typename T>
        struct GenericHash
        {
            size_t operator()(const T& guid) const 
            {
                // Combine multiple components of the T for a better hash
                size_t hashValue = 2166136261u; // FNV-1a offset basis

                const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&guid);
                for (size_t i = 0; i < sizeof(T); ++i) {
                    hashValue ^= bytes[i];
                    hashValue *= 16777619u; // FNV prime
                }

                return hashValue;
            }
        };
    }
}
