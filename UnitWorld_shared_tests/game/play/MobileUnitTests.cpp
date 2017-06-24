#include "CppUnitTest.h"
#include "game/play/MobileUnit.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{		
	TEST_CLASS(MobileUnitTests)
	{
	public:
		
		TEST_METHOD(constructor_shouldNotThrowAnyException)
		{
			MobileUnit unit;
		}

		TEST_METHOD(setDestination_shouldNotThrowAnyException)
		{
			MobileUnit unit;
			unit.setDestination(Vector2(2, 4));
		}
	};
}