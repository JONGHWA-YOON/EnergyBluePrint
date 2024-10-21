#pragma once
#include <unordered_map>
#include <functional>
#include <initializer_list>
#include "YunutyEngine.h"

/// 모든 게임 내 버튼에 대한 enum 클래스, 버튼 로직을 담고있는 클래스
/// 2023.08.23 윤종화

class ButtonLogicManager
{
public:
	enum class eButtonType
	{
		None,
		GameStart, Credit, GameQuit,
		Play, Stage, Back,
		Run, Menu,
		Continue, Restart, MenuQuit,
		NextStage,
		StageClear
	};

public:
	static ButtonLogicManager* instance;
	ButtonLogicManager();
	~ButtonLogicManager();

public:
	const std::unordered_map<eButtonType, std::function<void()>>& GetLogicMap();
	void OpenMainMenu();

	void UiCanvasLogic(std::string activeCanvas);

	// Main Menu
	void GameStartButton();
	void CreditButton();
	void GameQuitButton();

	// Stage Select
	void PlayButton();
	// int로 스테이지 몇단계인지 받을까 고민 중
	void StageButton();
	void BackButton();

	// Ingame
	void RunButton();
	void MenuButton();

	// Ingame Menu
	void ContinueButton();
	void RestartButton();
	void MenuQuitButton();

	// Stage Clear
	void NextStageButton();
	// restart 버튼은 Ingame Menu에 있는 것을 공유해서 사용해도 될 거 같음

	void StageClearLogic();

	void StageFailLogic();

	void DrawStageClearCanvas();

private:
	void SetLogicMap();

	std::unordered_map<eButtonType, std::function<void()>> m_buttonLogic;
};

// template <ButtonLogic::eButtonType ButtonType>
// void ButtonLogic::ButtonClick()
// {
// 	if constexpr (ButtonType == ButtonLogic::eButtonType::eGameStart)
// 	{
// 		int a = 5;
// 	}
// }

