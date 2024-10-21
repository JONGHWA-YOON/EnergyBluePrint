module;
#include "Component.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <string>
export module test.components.HailKimJungIl;



export class HailKimJungIl :
	public Component
{
public:
	enum class eTest
	{
		a,
		b,
		c,
		d,
		e
	};

	std::string lastOrder = "Last Order";
	bool haveNuclear = true;
	int wifeCount = 10;
	int swissBankAccountDollar = 20000;
	float Height = 160.1;
	yunutyEngine::Vector3d testVector = { 1.0f, 2.0f, 3.0f };
	eTest alphabet= eTest::c;

private:
	
};
