// Serialize example
// This example shows writing JSON string with writer directly.

#include "../Include/rapidjson/prettywriter.h" // for stringify JSON
#include "../Include/rapidjson/reader.h" // for stringify JSON
#include "../Include/rapidjson/document.h" // for stringify JSON
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
#include "Serialize.h"
#pragma comment(lib,"Rpcrt4.lib")

using namespace rapidjson;
using namespace std;

/// <summary>
/// 템플릿 내에 주어져있는 키 값들을 비교하는 변수
/// </summary>
template<typename T>
concept PairInstantiation = std::is_same_v<T, std::pair<typename T::first_type, typename T::second_type>>;

/// <summary>
/// 직접 만든 Handler 클래스.
/// Serialization을 구현하기 위해 만든 코드이다.
/// </summary>
/// <typeparam name="JsonStruct"></typeparam>
template <typename JsonStruct>
class JsonStructHandler
{
public:
    template<typename KeyPair, typename... KeyPairs>
        requires PairInstantiation<KeyPair>
    JsonStructHandler(KeyPair keyPair, KeyPairs... keypairs) : JsonStructHandler(keypairs...)
    {
        RegisterKeyValue(boolMap, keyPair);
        RegisterKeyValue(intMap, keyPair);
        RegisterKeyValue(uintMap, keyPair);
        RegisterKeyValue(int64Map, keyPair);
        RegisterKeyValue(uint64Map, keyPair);
        RegisterKeyValue(doubleMap, keyPair);
        //RegisterKeyValue(rawNumberMap, keyPair);
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
        //RegisterKeyValue(rawNumberMap, keyPair);
        RegisterKeyValue(stringMap, keyPair);
    }
    JsonStruct ReadData(const WCHAR* jsonStr, JsonStruct jsonStruct = JsonStruct())
    {
        rapidjson::GenericReader<rapidjson::UTF8<WCHAR>, rapidjson::UTF8<WCHAR>> reader;
        rapidjson::GenericStringStream<rapidjson::UTF8<WCHAR>> ss(jsonStr);
        targetStruct = &jsonStruct;
        JsonReadHandler readHandler{ this };
        reader.Parse(ss, readHandler);
        targetStruct = nullptr;

        return jsonStruct;
    }
    ///데이터를 직렬화해주는 함수.
    ///PrettyWriter 변수를 선언해서 serializeList에 있는 것들을 모두 write한다.
    ///
    wstring SerializeData(JsonStruct& target)
    {
        WStringBuffer stringBuffer;
        PrettyWWriter writer{ stringBuffer };
        targetStruct = &target;
        writer.StartObject();
        //for (decltype(serializeList)::reverse_iterator each : serializeList)
        for (auto itr = serializeList.rbegin(); itr != serializeList.rend(); itr++)
            (*itr)(writer);
        writer.EndObject();

        return stringBuffer.GetString();
    }

    /// <summary>
    /// 기본적인 자료형 값을 반환해주는 구조체. 보통 bool값으로 반환한다.
    /// </summary>
    struct JsonReadHandler
    {
        JsonReadHandler(JsonStructHandler<JsonStruct>* parent) : parent(parent) {}
        bool Null() { return true; }
        bool Bool(bool b) { parent->CallMappedFunction(parent->boolMap, b); return true; }
        bool Int(int i) { parent->CallMappedFunction(parent->intMap, i); return true; }
        bool Uint(unsigned u) { parent->CallMappedFunction(parent->uintMap, u); return true; }
        bool Int64(int64_t i) { parent->CallMappedFunction(parent->int64Map, i); return true; }
        bool Uint64(uint64_t u) { parent->CallMappedFunction(parent->uint64Map, u); return true; }
        bool Double(double d) { parent->CallMappedFunction(parent->doubleMap, d); return true; }
        bool RawNumber(const WCHAR* str, SizeType length, bool copy) { parent->CallMappedFunction(parent->rawNumberMap, wstring(str)); return true; }
        bool String(const WCHAR* str, SizeType length, bool copy) { parent->CallMappedFunction(parent->stringMap, wstring(str)); return true; }
        bool StartObject() { return true; }
        bool Key(const WCHAR* str, SizeType length, bool copy) { parent->key = wstring(str); return true; }
        bool EndObject(SizeType memberCount) { return true; }
        bool StartArray() { return true; }
        bool EndArray(SizeType elementCount) { return true; }
    private:
        JsonStructHandler<JsonStruct>* parent;
    };
private:
    /// <summary>
    ///serialization하는데에 필요한 변수들 기본적으로 Key값들을 리스트로 저장해놓는 것들이다.
    /// </summary>
    using WStringBuffer = GenericStringBuffer<rapidjson::UTF8<WCHAR>>;
    using PrettyWWriter = PrettyWriter<WStringBuffer, rapidjson::UTF8<WCHAR>, rapidjson::UTF8<WCHAR>>;
    std::map<wstring, function<void(bool)>> boolMap;
    std::map<wstring, function<void(int i)>> intMap;
    std::map<wstring, function<void(unsigned u)>> uintMap;
    std::map<wstring, function<void(int64_t i)>> int64Map;
    std::map<wstring, function<void(uint64_t u)>> uint64Map;
    std::map<wstring, function<void(double d)>> doubleMap;
    std::map<wstring, function<void(wstring)>> rawNumberMap;
    std::map<wstring, function<void(wstring)>> stringMap;
    std::vector<function<void(PrettyWWriter&)>> serializeList;

    ///Key값을 리스트에 저장하는 함수 템플릿.
    ///인자로 들어가는 값의 자료형을 판별하여 그에 맞는 리스트에 저장한다.
    template<typename ValueType, typename KeyPair>
    void RegisterKeyValue(std::map<wstring, function<void(ValueType)>>& funcMap, const KeyPair& keyPair)
    {
        //static_assert(is_same_v<decltype(keyPair.first), wstring>, "Yunu : json key value type should be wstring");
        
        /// 페어의 두번째 변수가 멤버 변수 포인터일때
        if constexpr (is_same_v<decltype(keyPair.second), ValueType JsonStruct::*>)
        {
            funcMap[keyPair.first] = [=](ValueType arg) {this->targetStruct->*keyPair.second = arg; };
            serializeList.push_back(
                [=](PrettyWWriter& writer)
                {
                    ///자료형 별로 타입이 입력되었을 때 그에 맞는 writer를 호출한다.
                    writer.String(keyPair.first);
                    if constexpr (is_same_v<bool, ValueType>) { writer.Bool(this->targetStruct->*(keyPair.second)); };
                    if constexpr (is_same_v<int, ValueType>) { writer.Int(this->targetStruct->*(keyPair.second)); };
                    if constexpr (is_same_v<unsigned, ValueType>) { writer.Uint(this->targetStruct->*(keyPair.second)); };
                    if constexpr (is_same_v<int64_t, ValueType>) { writer.Int64(this->targetStruct->*(keyPair.second)); };
                    if constexpr (is_same_v<uint64_t, ValueType>) { writer.Uint64(this->targetStruct->*(keyPair.second)); };
                    if constexpr (is_same_v<double, ValueType>) { writer.Double(this->targetStruct->*(keyPair.second)); };
                    if constexpr (is_same_v<wstring, ValueType>) { writer.String((this->targetStruct->*(keyPair.second)).c_str()); };
                } 
            );
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
                    ///자료형 별로 타입이 입력되었을 때 그에 맞는 writer를 호출한다.
                    writer.String(keyPair.first);
                    if constexpr (is_same_v<bool, ValueType>) { writer.Bool(this->targetStruct->*(keyPair.second)()); };
                    if constexpr (is_same_v<int, ValueType>) { writer.Int(this->targetStruct->*(keyPair.second)()); };
                    if constexpr (is_same_v<unsigned, ValueType>) { writer.Uint(this->targetStruct->*(keyPair.second)()); };
                    if constexpr (is_same_v<int64_t, ValueType>) { writer.Int64(this->targetStruct->*(keyPair.second)()); };
                    if constexpr (is_same_v<uint64_t, ValueType>) { writer.Uint64(this->targetStruct->*(keyPair.second)()); };
                    if constexpr (is_same_v<double, ValueType>) { writer.Double(this->targetStruct->*(keyPair.second)()); };
                    if constexpr (is_same_v<wstring, ValueType>) { writer.String((this->targetStruct->*(keyPair.second)()).c_str()); };
                }
            );
            funcMap[keyPair.first] = [=](ValueType arg) {(this->targetStruct->*(keyPair.second)())(arg); };
        }
    };
private:
    template<typename T>
    void CallMappedFunction(std::map<wstring, function<void(T)>>& funcMap, const T& value)
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
    A a, b;
    a.b = true;
    JsonStructHandler<A> handler(std::pair{ L"A", &A::a }, std::pair{ L"B",&A::str });
    a = handler.ReadData(L"{\"A\":13,\"B\":\"string is right here\"}");
    b = handler.ReadData(handler.SerializeData(a).c_str());

    Bjson::Serialize _test;

    _test.SceneSerialize("test");
};
