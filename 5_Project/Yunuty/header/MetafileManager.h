#pragma once
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <map>
#include <unordered_set>
#include <string>
#include "ResourceType.h"
#include "SingletonClass.h"

using namespace std;
namespace yunutyEngine
{
    namespace resource
    {
        class MetaData;
        class MetafileManager : SingletonClass<MetafileManager>
        {
        public:
            static MetafileManager& SingleInstance();
            void GetOrCreateMetaFiles(const WCHAR* rootPath);
            static const map<wstring, ResourceType> ResourceTypeByExtension;
            static const unordered_set<wstring> resourceExtensions;
        private:
            MetafileManager() = default;
            //static unique_ptr<MetafileManager> instance;
            vector<MetaData> loadedMetaDatas;

            MetaData GenerateMetaFile(const WCHAR* fileName);
            MetaData LoadMetaFile(const WCHAR* fileName);
        };
    }
}
