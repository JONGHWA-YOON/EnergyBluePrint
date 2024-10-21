#pragma once
#include "IUnknown.h"
namespace yunuGI
{
	class IRenderable;
	// EventHandler는 그래픽스 엔진 내부에서만 처리할수 있는
	// 이벤트(클릭, 컬링 등)에 대한 콜백 함수를 저장하고, 이벤트 요청을 처리한다.
    struct IEventHandlerDesc
    {
    };
	class IEventHandler : public IUnknown
	{
	public:
		typedef void(*CallbackPtr)(IRenderable*);
		virtual void AddClickCallback(CallbackPtr callback) {};

		// call ClickCallback function registered by clients request,
		// considering origin point is located at topleft, and x,y coordinates are clamped between 1.0 and 0.0
		virtual yunuGI::IRenderable* Click(float normalizedX, float normalizedY) { return nullptr; }
	};
}