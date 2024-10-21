/// 2023. 07. 09. 김상준
/// 게임과 별개로 에디터 편집을 위해 사용하는 카메라 시점에서의 화면을 보여주는 패널
/// 싱글턴이므로 EditorCamera 에 직접 접근함
 
import ui.EditorPanel;
export module ui.ScenePanel;

export class ScenePanel
	: public EditorPanel
{
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnUpdate(float ts) override;
	virtual void DrawPanel() override;

private:
	void ReturnMousePos();

private:
	float m_gameImageSizeX = 0.0f;
	float m_gameImageSizeY = 0.0f;
};
