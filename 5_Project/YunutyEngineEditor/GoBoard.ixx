module;
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "YunutyEngine.h"
#include <vector>
export module htkg.components.GoBoard;


export class GoBoard :
	public Component
{
public:
	int GetHeight();
	int GetWidth();
	void SetHeight(int size);
	void SetWidth(int size);
	void SetBoard(yunutyEngine::GameObject* obj);
	void SetInterval();
	int m_goalHeightSize = 0;
	int m_goalWidthSize = 0;
	float m_interval = 0.0f;

private:
	std::vector<std::vector<yunutyEngine::GameObject*>> m_goBoard;
	yunutyEngine::GameObject* parentObject = nullptr;
	int m_prevBoardSizeH = 0;
	int m_prevBoardSizeW = 0;
	float m_prevInterval = 1.1f;
	bool m_InitFlag = true;
};
