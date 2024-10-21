// Serialize example
// This example shows writing JSON string with writer directly.

#include "../RapidJson/header/rapidjson/prettywriter.h" // for stringify JSON
#include "../RapidJson/header/rapidjson/reader.h" // for stringify JSON
#include <cstdio>
#include <string>
#include <vector>
#include <rpc.h>
#include <rpcdce.h>
#include <iostream>
#include <functional>
#include <map>
#include <concepts>
#include <utility>
#pragma comment(lib,"Rpcrt4.lib")

using namespace rapidjson;
using namespace std;

template<typename T>
concept PairInstantiation = std::is_same_v<T, std::pair<typename T::first_type, typename T::second_type>>;

template <typename JsonStruct>
class JsonStructHandler
{
public:
    template<typename KeyPair, typename... KeyPairs>
        requires PairInstantiation<KeyPair>
    JsonStructHandler(KeyPair keyPair, KeyPairs... keypairs) : JsonStructHandler(keypairs...)
    {
        RegisterKeyValue<bool, KeyPair>(boolMap, keyPair);
        RegisterKeyValue(intMap, keyPair);
        RegisterKeyValue(uintMap, keyPair);
        RegisterKeyValue(int64Map, keyPair);
        RegisterKeyValue(uint64Map, keyPair);
        RegisterKeyValue(doubleMap, keyPair);
        RegisterKeyValue(rawNumberMap, keyPair);
        RegisterKeyValue(stringMap, keyPair);
    }
    template<typename KeyPair>
        requires PairInstantiation<KeyPair>
    JsonStructHandler(KeyPair keyPair)
    {
        RegisterKeyValue(boolMap, keyPair);
        RegisterKeyValue(intMap, keyPair);
        RegisterKeyValue(uintMap, keyPair);
        RegisterKeyValue(int64Map, keyPair);
        RegisterKeyValue(uint64Map, keyPair);
        RegisterKeyValue(doubleMap, keyPair);
        RegisterKeyValue(rawNumberMap, keyPair);
        RegisterKeyValue(stringMap, keyPair);
    }
    JsonStruct ReadData(const WCHAR* jsonStr, JsonStruct jsonStruct = JsonStruct())
    {
        rapidjson::GenericReader<rapidjson::UTF8<WCHAR>, rapidjson::UTF8<WCHAR>> reader;
        rapidjson::GenericStringStream<rapidjson::UTF8<WCHAR>> ss(jsonStr);
        targetStruct = &jsonStruct;
        reader.Parse(ss, *this);
        targetStruct = nullptr;

        return jsonStruct;
    }
    wstring SerializeData()
    {
        WStringBuffer stringBuffer;
        PrettyWWriter writer{ stringBuffer };
        writer.StartObject();
        for (auto each : serializeList)
            each(writer);
        writer.EndObject();

        return stringBuffer.GetString();
    }
    bool Null() { return true; }
    bool Bool(bool b) { CallMappedFunction(boolMap, b); return true; }
    bool Int(int i) { CallMappedFunction(intMap, i); return true; }
    bool Uint(unsigned u) { CallMappedFunction(uintMap, u); return true; }
    bool Int64(int64_t i) { CallMappedFunction(int64Map, i); return true; }
    bool Uint64(uint64_t u) { CallMappedFunction(uint64Map, u); return true; }
    bool Double(double d) { CallMappedFunction(doubleMap, d); return true; }
    bool RawNumber(const WCHAR* str, SizeType length, bool copy) { CallMappedFunction(rawNumberMap, wstring(str)); return true; }
    bool String(const WCHAR* str, SizeType length, bool copy) { CallMappedFunction(stringMap, wstring(str)); return true; }
    bool StartObject() { return true; }
    bool Key(const WCHAR* str, SizeType length, bool copy) { key = wstring(str); return true; }
    bool EndObject(SizeType memberCount) { return true; }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { return true; }
private:
    using WStringBuffer = GenericStringBuffer<rapidjson::UTF8<WCHAR>>;
    using PrettyWWriter = PrettyWriter<WStringBuffer, rapidjson::UTF8<WCHAR>, rapidjson::UTF8<WCHAR>>;
    std::map<wstring, function<void(bool)>> boolMap;
    std::map<wstring, function<void(int i)>> intMap;
    std::map<wstring, function<void(unsigned u)>> uintMap;
    std::map<wstring, function<void(int64_t i)>> int64Map;
    std::map<wstring, function<void(uint64_t u)>> uint64Map;
    std::map<wstring, function<void(double d)>> doubleMap;
    std::map<wstring, function<void(const wstring&)>> rawNumberMap;
    std::map<wstring, function<void(const wstring&)>> stringMap;
    std::vector<function<void(PrettyWWriter&)>> serializeList;

    template<typename ValueType, typename KeyPair>
    void RegisterKeyValue(std::map<wstring, function<void(ValueType)>>& funcMap, const KeyPair& keyPair)
    {
        //static_assert(is_same_v<decltype(keyPair.first), wstring>, "Yunu : json key value type should be wstring");
        if constexpr (is_same_v<decltype(keyPair.second), ValueType JsonStruct::*>)
        {
            funcMap[keyPair.first] = [=](ValueType arg) {this->targetStruct->*keyPair.second = arg; };
        }
        // 페어의 두번째 변수가 멤버 함수 포인터 중 Setter일때
        if constexpr (is_same_v<decltype(keyPair.second), void (JsonStruct::*)(ValueType)>)
        {
            funcMap[keyPair.first] = [=](ValueType arg) {(this->targetStruct->*(keyPair.second))(arg); };
        }
        // 페어의 두번째 변수가 멤버 함수 포인터 중 Getter일때
        if constexpr (is_same_v<decltype(keyPair.second), ValueType(JsonStruct::*)()>)
        {
            serializeList.push_back(
                [=](PrettyWWriter& writer)
                {
                    writer.String(keyPair.first);
                    if constexpr (is_same_v<bool, ValueType>) { writer.Bool((this->targetStruct->*(keyPair.second))()); };
                }
            );
            funcMap[keyPair.first] = [=](ValueType arg) {(this->targetStruct->*(keyPair.second))(arg); };
        }
    };
    template<typename KeyPair>
    void RegisterKeyValue(std::map<wstring, function<void(const wstring&)>>& funcMap, const KeyPair& keyPair)
    {
        //static_assert(is_same_v<decltype(keyPair.first), wstring>, "Yunu : json key value type should be string");
        if constexpr (is_same_v<decltype(keyPair.second), wstring JsonStruct::*>)
        {
            funcMap[keyPair.first] = [=](const wstring& arg) {this->targetStruct->*keyPair.second = arg; };
        }
        if constexpr (is_same_v<decltype(keyPair.second), void (JsonStruct::*)(wstring)>)
        {
            funcMap[keyPair.first] = [=](const wstring& arg) {(this->targetStruct->*(keyPair.second))(arg); };
        }
    };
private:
    template<typename T>
    void CallMappedFunction(std::map<wstring, function<void(T)>>& funcMap, const T& value)
    {
        assert(funcMap.find(key) != funcMap.end(), "yunu : json key string mismatch while reading.");
        funcMap[key](value);
    }
    void CallMappedFunction(std::map<wstring, function<void(const wstring&)>>& funcMap, const wstring& value)
    {
        assert(funcMap.find(key) != funcMap.end(), "yunu : json key string mismatch while reading.");
        funcMap[key](value);
    }
    JsonStruct* targetStruct;
    wstring key;
};
struct A
{
    unsigned a;
    bool b;
    wstring str;
    void SetB(bool val) { b = val; }
};
int main()
{
    A a;
    a.b = true;
    JsonStructHandler<A> handler(std::pair{ L"A", &A::a }, std::pair{ L"B",&A::str });
    a = handler.ReadData(L"{\"A\":13,\"B\":\"string is right here\"}");
};
