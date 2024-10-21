#pragma once
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <codecvt>
#include "ResourceType.h"
#include "MetafileManager.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"
#include "rapidjson/ostreamwrapper.h"

using namespace std;
namespace yunutyEngine
{
    namespace resource
    {
        constexpr int MetafileVersion{ 1 };
        class MetaData
        {
        public:
            wstring filePath;
            GUID guid;
            ResourceType resourceType;
            int metaFileVersion;

            //template <typename Writer>
            wstring SerializeToJson(); 
            static MetaData DeserializeFromJson(const WCHAR* jsonStr);
        private:
        };
    };

}
