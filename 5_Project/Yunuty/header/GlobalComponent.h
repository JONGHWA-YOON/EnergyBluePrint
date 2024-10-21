#pragma once
#include "Object.h"
#include <vector>
#include <set>
#include <memory>
#include <map>
#include <unordered_set>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;

namespace yunutyEngine
{
    // Global components are always called before other components.
    class YUNUTY_API GlobalComponent :
        public Object
    {
        friend class YunutyCycle;
    private:
        //static std::map<GlobalComponent*,unique_ptr<GlobalComponent>> globalComponents;
        static std::unordered_set<GlobalComponent*> globalComponents;
    protected:
        GlobalComponent();
        virtual void Update() {};
        virtual void FixedUpdate() {};
    public:
        virtual ~GlobalComponent();
    };
}

