#pragma once
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
#include <fstream>
#include <unordered_map>
#pragma comment(lib,"Rpcrt4.lib")

using namespace rapidjson;
using namespace std;

/// <summary>
/// 변지상의 Serialize 늦게나마 작업 시작
/// 2023.08.17
/// </summary>
namespace Bjson
{
	class Serialize
	{
	public:
		Serialize();
		~Serialize();

		//씬이 생성되었을 때 직렬화 할 함수
		void SceneSerialize(const std::string& name);

		//오브젝트가 생성되었을 때 직렬화 할 함수
		void ObjectSerialize(Document& doc, const std::string& name);

		void LoadSerializeDataToFile(StringBuffer& buffer, const char* filePath);

	private:
		/// <summary>
		/// 각각의 데이터를 벡터 형식으로 들고있음
		/// </summary> 
		std::unordered_map<Value*, std::string> sceneDataList;
		std::vector<Value*> objectDataList;
	};
}