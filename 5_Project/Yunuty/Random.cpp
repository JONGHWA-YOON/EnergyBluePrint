#include "Random.h"

using namespace yunutyEngine;
using namespace yunutyEngine::math;

Random* Random::instance = nullptr;
Random::Random()
{
    gen = mt19937(rd());
}
int Random::GetRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dis(min, max);
    return dis(SingleInstance()->gen);
}
Random* Random::SingleInstance()
{
    if (instance == nullptr)
        instance = new Random();
    return instance;
}
