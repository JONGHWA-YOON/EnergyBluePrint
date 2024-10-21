#pragma once
#include <iostream>
#include "Object.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif


namespace yunutyEngine
{
    template<typename real>
    class YUNUTY_API Vector  abstract: Object
    {
    protected:
        real * vectorArray;
        unsigned int vectorSize;
        Vector(real* vectorArray, unsigned int vectorSize) : vectorArray(vectorArray), vectorSize(vectorSize) {}
        Vector(const Vector& rhs) = delete;
        void Normalize()
        {
            //for(int i=0;)
//vectorArray
        }
    public:


        Vector& operator=(const Vector& rhs)
        {
            for (unsigned int i = 0; i < vectorSize; i++)
                vectorArray[i] = i < rhs.vectorSize ? rhs.vectorArray[i] : 0;
            return *this;
        }
        Vector& operator+=(const Vector& rhs)
        {
            for (unsigned int i = 0; i < vectorSize; i++)
                vectorArray[i] += i < rhs.vectorSize ? rhs.vectorArray[i] : 0;
            return *this;
        }
        Vector& operator-=(const Vector& rhs)
        {
            for (unsigned int i = 0; i < vectorSize; i++)
                vectorArray[i] -= i < rhs.vectorSize ? rhs.vectorArray[i] : 0;
            return *this;
        }
        Vector& operator*=(const double& scalar)
        {
            for (unsigned int i = 0; i < vectorSize; i++)
                vectorArray[i] *= scalar;
            return *this;
        }
        Vector& operator/=(const double& scalar)
        {
            for (unsigned int i = 0; i < vectorSize; i++)
                vectorArray[i] /= scalar;
            return *this;
        }
        /*template<typename SubVector>
        operator SubVector()const
        {
            static_assert(std::is_base_of<Vector, SubVector>::value, "only derived classes from vector are allowed");
            SubVector ret;
            (Vector)ret = *this;
            return ret;
        }
        template<class SubVector>
        operator const SubVector& ()const
        {
            return (SubVector&)*this;
        }*/
    };
}
