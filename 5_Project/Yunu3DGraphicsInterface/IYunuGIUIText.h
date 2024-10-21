#pragma once
#include <windows.h>
#include <string>
#include "IYunuGIUIObject.h"

namespace yunuGI
{
	class IUIText : public yunuGI::IUIObject
	{
	public:
		virtual void SetText(TCHAR* text, ...) = 0;
		virtual const std::string GetText() = 0;
	};
	struct IUITextDesc
	{

	};
}
