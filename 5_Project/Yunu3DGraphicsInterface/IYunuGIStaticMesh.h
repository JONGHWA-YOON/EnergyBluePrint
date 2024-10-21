#pragma once
#include "IYunuGIRenderable.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

namespace yunuGI
{
	class IStaticMesh : virtual public yunuGI::IRenderable
	{
	public:
		virtual void LoadMesh(const char* fileName) = 0;
		virtual void LoadDiffuseMap(const char* fileName) = 0;
		virtual void LoadNormalMap(const char* fileName) = 0;
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
	};

	struct IStaticMeshDesc
	{
	};
}
