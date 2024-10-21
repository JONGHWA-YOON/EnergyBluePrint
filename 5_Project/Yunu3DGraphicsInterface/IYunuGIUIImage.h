#pragma once
#include <windows.h>
#include "IYunuGIUIObject.h"

namespace yunuGI
{
	class IUIImage : public yunuGI::IUIObject
	{
	public:
		virtual void SetImage(LPCSTR fileName) = 0;
	};
	struct IUIImageDesc
	{
		
	};
}
