#include <locale>
#include <codecvt>
#include "UIManager.h"

import ui.DebugPanel;
import ui.InspectorPanel;

UIManager* UIManager::instance = nullptr;

UIManager::UIManager()
{
	m_pButtonManager = new ButtonLogicManager();
}

UIManager::~UIManager()
{

}

UIManager* UIManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new UIManager();
	}
	return instance;
}

void UIManager::Delete()
{
	delete instance;
}

void UIManager::CreateImageButton(float posX, float posY, float sclX, float sclY, bool isAcitvate, std::string objectName, yunutyEngine::GameObject* parentObject, std::string imageSource, ButtonLogicManager::eButtonType buttonType)
{
	auto uiImageObject = Scene::getCurrentScene()->AddGameObject();
	InspectorPanel::imageList.insert(std::make_pair(uiImageObject, imageSource));
	InspectorPanel::buttonTypeList.insert(std::make_pair(uiImageObject, buttonType));
	if (objectName.empty() == true)
	{
		std::string buttonName = "UnNamed";
		uiImageObject->setName(buttonName);
	}
	else
	{
		uiImageObject->setName(objectName.c_str());
	}

	if (parentObject != nullptr)
	{
		uiImageObject->SetParent(parentObject);
	}

	auto uiImageComp = uiImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
	uiImageComp->GetGI().SetImage(InspectorPanel::imageList.at(uiImageObject).c_str());
	uiImageComp->GetTransform()->SetWorldPosition(Vector3d(posX, posY, 0.0f));
	uiImageComp->GetTransform()->scale = Vector3d(sclX, sclY, 0.0f);

	auto uiEventHandler = uiImageObject->AddComponent<yunutyEngine::graphics::EventHandler>();
	uiEventHandler->SetClickCallback([=]() {
		if (m_pButtonManager->GetLogicMap().find(InspectorPanel::buttonTypeList.at(uiImageObject)) != m_pButtonManager->GetLogicMap().end())
		{
			if (m_pButtonManager->GetLogicMap().at(InspectorPanel::buttonTypeList.at(uiImageObject)) != nullptr)
			{
				auto buttonLogic = m_pButtonManager->GetLogicMap().at(InspectorPanel::buttonTypeList.at(uiImageObject));
				buttonLogic();
			}
		}
		DebugPanel::LogMessage((uiEventHandler->GetGameObject()->getName() + " clicked").c_str());
		});

	uiImageObject->SetSelfActive(isAcitvate);
}

void UIManager::CreateImage(float posX, float posY, float sclX, float sclY, bool isAcitvate, std::string objectName, yunutyEngine::GameObject* parentObject, std::string imageSource)
{
	auto uiImageObject = Scene::getCurrentScene()->AddGameObject();

	InspectorPanel::imageList.insert(std::make_pair(uiImageObject, imageSource));
	if (objectName.empty() == true)
	{
		std::string objectName = "UnNamed";
		uiImageObject->setName(objectName);
	}
	else
	{
		uiImageObject->setName(objectName.c_str());
	}

	if (parentObject != nullptr)
	{
		uiImageObject->SetParent(parentObject);
	}

	auto uiImageComp = uiImageObject->AddComponent<yunutyEngine::graphics::UIImage>();
	
	uiImageComp->GetGI().SetImage(imageSource.c_str());
	uiImageComp->GetGI().SetPickingMode(false);
	//if (InspectorPanel::imageList.find(uiImageObject) != InspectorPanel::imageList.end())
	//{
	//	uiImageComp->GetGI().SetImage(InspectorPanel::imageList.at(uiImageObject).c_str());
	//}
	uiImageComp->GetTransform()->SetWorldPosition(Vector3d(posX, posY, 0.0f));
	uiImageComp->GetTransform()->scale = Vector3d(sclX, sclY, 0.0f);

	uiImageObject->SetSelfActive(isAcitvate);
}

void UIManager::CreateTextButton(float posX, float posY, float sclX, float sclY, bool isAcitvate, std::string objectName, yunutyEngine::GameObject* parentObject, TCHAR* text, ButtonLogicManager::eButtonType buttonType)
{
	auto uiTextObject = Scene::getCurrentScene()->AddGameObject();
	uiTextObject->setName(objectName);
	if (parentObject != nullptr)
	{
		// 부모 넣으면 Activate 작동 안함 / 크기도 뒤바뀜 -> 부모 처리 안하도록.
		//uiTextObject->SetParent(parentObject);
	}
	auto uiTextComp = uiTextObject->AddComponent<yunutyEngine::graphics::UIText>();;

	uiTextComp->GetGI().SetText(text);

	uiTextComp->GetTransform()->SetWorldPosition(Vector3d(posX, posY, 0.0f));
	uiTextComp->GetTransform()->scale = Vector3d(sclX, sclY, 0.0f);
	
	//uiTextComp->GetGI().SetWorldSpace();

	auto uiEventHandler = uiTextObject->AddComponent<yunutyEngine::graphics::EventHandler>();
	uiEventHandler->SetClickCallback([=]() {
		if (m_pButtonManager->GetLogicMap().find(InspectorPanel::buttonTypeList.at(uiTextObject)) != m_pButtonManager->GetLogicMap().end())
		{
			if (m_pButtonManager->GetLogicMap().at(InspectorPanel::buttonTypeList.at(uiTextObject)) != nullptr)
			{
				auto buttonLogic = m_pButtonManager->GetLogicMap().at(InspectorPanel::buttonTypeList.at(uiTextObject));
				buttonLogic();
			}
		}
		DebugPanel::LogMessage((uiEventHandler->GetGameObject()->getName() + " clicked").c_str());
		});

	uiTextObject->SetSelfActive(isAcitvate);
}
