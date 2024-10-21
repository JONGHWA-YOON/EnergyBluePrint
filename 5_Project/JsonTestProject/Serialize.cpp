#include "Serialize.h"

Bjson::Serialize::Serialize()
{
}

Bjson::Serialize::~Serialize()
{
}

/// <summary>
/// 여기서 매개변수로 그냥 단순 string값이 아니라
/// Scene에 존재하는 이름을 받아오면 되지 않을까...?
/// </summary>
/// <param name="name"></param>
void Bjson::Serialize::SceneSerialize(const std::string& name)
{
	//씬 데이터 생성
	Document doc(kArrayType);
	doc.SetArray();

	Value sceneVal(kObjectType);

	Value sceneName;
	sceneName.SetString(name.c_str(), strlen(name.c_str()), doc.GetAllocator());
	sceneVal.AddMember(StringRef("Scene Name"), sceneName, doc.GetAllocator());

	for (auto iter : objectDataList)
	{
	}
	///씬 데이터 안에 오브젝트를 넣고 싶은데....
	ObjectSerialize(doc, "Object1");
	
	doc.PushBack(sceneVal, doc.GetAllocator());
	
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	doc.Accept(writer);

	cout << buffer.GetString() << endl;
}

/// <summary>
/// 이것도 마찬가지로 오브젝트의 이름을 매개변수로 받아오면 될 것 같은데...
/// </summary>
/// <param name="name"></param>
void Bjson::Serialize::ObjectSerialize(Document& doc, const std::string& name)
{
	Value objectVal(kObjectType);

	Value objName;
	objName.SetString(name.c_str(), strlen(name.c_str()), doc.GetAllocator());

	objectVal.AddMember(StringRef("Object Name"), objName, doc.GetAllocator());

	doc.PushBack(objectVal, doc.GetAllocator());
}

///Serialize한 데이터를 파일로 출력하는 함수
void Bjson::Serialize::LoadSerializeDataToFile(StringBuffer& buffer, const char* filePath)
{
	std::ofstream outputFile(filePath);

	if (outputFile.is_open())
	{
		outputFile << buffer.GetString();
		outputFile.close();
	}
	else
	{
		assert(outputFile.is_open());
	}
}
