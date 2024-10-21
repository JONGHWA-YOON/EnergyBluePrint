#pragma once
#include "Component.h"
#include "YunutyRenderable.h"
#include "YunuGraphicsInterface.h"

// 카메라는 화면에 게임상에 존재하는 모든 그래픽 요소들을 출력하는 데에 쓰이는 객체입니다. 
// 카메라의 구현은 엔진의 동작환경,혹은 카메라의 특성에 따라 달라질 수 있기 때문에,
// 화면을 그려내기 위해 쓰이는 함수 Render는 추상 메소드로 정의됩니다.
#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine::graphics
{
	class YunutyCycle;
	class YUNUTY_API UIImage : public Renderable<yunuGI::IUIImage>
	{
	private:
	protected:
		virtual void Update() override;
	public:
		// GI는 Graphics Interface라는 뜻임.
		yunuGI::IUIImage& GetGI() { return Renderable<yunuGI::IUIImage>::GetGI(); }
		UIImage();
	};
}
