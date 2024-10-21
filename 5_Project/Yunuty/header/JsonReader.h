//#pragma once
//#include <iostream>
//#include <wchar.h>
//#include "ResourceType.h"
//
//using namespace std;
//namespace yunutyEngine::json
//{
//    // 템플릿으로 어떤 key값이 어떤 주소와 대응되는지 정의한다.
//    //template<std::pair<const wchar_t*, void*>... keyAddressPairs>
//    //template<typename... Args>
//    ////template<typename T,typename keyAddressPair,typename... keyAddressPairs>
//    //template<std::pair<const wchar_t*, Args>... keyAddressPairs>
//    //struct JsonTest
//    //{
//
//    //};
//    template<typename JsonStructType, typename... KeyAddressPair>
//    class JsonReader
//    {
//    public:
//        JsonReader(JsonStructType& readTarget) :readTarget(readTarget) {}
//
//        //bool Null() { return true; }
//        //bool Bool(bool b) { return true; }
//        //bool Int(int i) { return true; }
//        //bool Uint(unsigned u) { return true; }
//        //bool Int64(int64_t i) { return true; }
//        //bool Uint64(uint64_t u) { return true; }
//        //bool Double(double d) { return true; }
//        //bool RawNumber(const WCHAR* str, SizeType length, bool copy) { return true; }
//        //bool String(const WCHAR* str, SizeType length, bool copy) { return true; }
//        //bool StartObject() { return true; }
//        //bool Key(const WCHAR* str, SizeType length, bool copy) { return true; }
//        //bool EndObject(SizeType memberCount) { return true; }
//        //bool StartArray() { return true; }
//        //bool EndArray(SizeType elementCount) { return true; }
//    private:
//        JsonStructType& readTarget;
//    };
//    // DataType의 값을 JsonStructType 객체의 알맞은 멤버변수 주소에 key값을 대조해가며 집어넣는다.
//    //template<typename JsonStructType, typename DataType, typename KeyAddressPair, typename... KeyAddressPairs>
//    template<typename JsonStructType, typename DataType, typename KeyAddressPair, typename... KeyAddressPairs>
//    struct AssignData
//    {
//        template<>
//        static void AssignDataToJsonStruct(JsonStructType& targetStruct, const DataType& data, const wchar_t* key)
//        {
//        };
//        template<KeyAddressPair keyAddressPair, KeyAddressPairs... keyAddressPairs>
//        static void AssignDataToJsonStruct(JsonStructType& targetStruct, const DataType& data, const wchar_t* key)
//        {
//            AssignData<JsonStructType, DataType, KeyAddressPairs>::AssignDataToJsonStruct<keyAddressPairs...>();
//        }
//    };
//}
////template<std::pair<const wchar_t*, void*> keyAddressPair, std::pair<const wchar_t*, void*>... keyAddressPairs>
////template<typename T>
//template<typename JsonStructType, typename DataType, typename KeyAddressPair, typename... KeyAddressPairs>
//void AssignDataToJsonStruct(JsonStructType& targetStruct, const DataType& data, const wchar_t* key)
//{
//    if constexpr (is_same<int, decltype(ad)>())
//    {
//
//    }
//}
//template<typename JsonStructType, typename T2, typename... KeyAddressPair>
//void yunutyEngine::resource::JsonReader<JsonStructType, T2, KeyAddressPair...>::AssignData(KeyAddressPair...) {}
//template<typename JsonStructType, typename... KeyAddressPair>
//void yunutyEngine::resource::JsonReader<JsonStructType, KeyAddressPair...>::AssignData(KeyAddressPair...)
//{
//
//    if (wcscmp(keyValue, keyAddressPair.first) == 0)
//    {
//        *static_cast<JsonStructType*>(keyAddressPair.second) = value;
//        return;
//    }
//    JsonReader<keyAddressPairs...>::AssignData<JsonStructType>(keyValue, value);
//}
