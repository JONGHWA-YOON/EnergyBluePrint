#include "MetaData.h"
//#include "JsonReadHandler.h"

using namespace yunutyEngine::resource;
using namespace rapidjson;

wstring yunutyEngine::resource::MetaData::SerializeToJson()
{
    rapidjson::GenericStringBuffer<rapidjson::UTF8<WCHAR>> sb;
    rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF8<WCHAR>>> writer{ sb };

    // This base class just write out name-value pairs, without wrapping within an object.
    RPC_CSTR guidRPCSTR = nullptr;
    std::string guidStdStr((char*)guidRPCSTR);
    UuidToStringA(&guid, &guidRPCSTR);

    writer.StartObject();

    writer.String("metaFileVersion");
    writer.Uint(metaFileVersion);

    writer.String("guid");
    writer.String(guidStdStr.c_str(), static_cast<rapidjson::SizeType>(guidStdStr.length())); // Supplying length of string is faster.

    writer.String("resourceType");
    writer.Uint((unsigned int)resourceType);

    writer.EndObject();
    sb.GetString();
    return sb.GetString();
};
//MetaData YunutyEngine::Resource::MetaData::DeserializeFromJson(const WCHAR* jsonStr)
//{
//    MetaData metaData;
//    int MetaData::* abd = &MetaData::metaFileVersion;
//    JsonTest < MetaData, std::make_pair(1,2) > abcd;
//    void MetaData::* acc = abd;
//    JsonReader<std::pair(L"a", &MetaData::metaFileVersion)>
//        ////MyHandler handler;
//        //rapidjson::GenericReader<rapidjson::UTF8<WCHAR>, rapidjson::UTF8<WCHAR> > reader;
//        //rapidjson::GenericStringStream<rapidjson::UTF8<WCHAR>> ss(jsonStr);
//        //ss.
//        //    //rapidjson::StringStream<UTF16<>> assd;
//        //    //rapidjson::StringStream ss(json);
//        //    reader.Parse(ss, handler);
//        //// This base class just write out name-value pairs, without wrapping within an object.
//        //RPC_CSTR guidRPCSTR;
//        //std::string guidStdStr((char*)guidRPCSTR);
//        //UuidToStringA(&guid, &guidRPCSTR);
//
//        //writer.StartObject();
//
//        //writer.String("metaFileVersion");
//        //writer.Uint(metaFileVersion);
//
//        //writer.String("guid");
//        //writer.String(guidStdStr.c_str(), static_cast<rapidjson::SizeType>(guidStdStr.length())); // Supplying length of string is faster.
//
//        //writer.String("resourceType");
//        //writer.Uint((unsigned int)resourceType);
//
//        //writer.EndObject();
//        //sb.GetString();
//        //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//        //return converter.from_bytes(sb.GetString());
//        return metaData;
//}
