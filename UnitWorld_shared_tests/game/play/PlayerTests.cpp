#include "CppUnitTest.h"
#include "game/play/Player.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{		
	TEST_CLASS(PlayerTests)
	{
	public:
		
		TEST_METHOD(constructor_shouldNotThrowAnyException)
		{
			Player testPlayer;
		}

		TEST_METHOD(setMobileUnitsDestinationInArea_shouldNotThrowAnyException)
		{
			Player testPlayer;
			testPlayer.setMobileUnitsDestinationInArea(Rectangle(), Point());
		}
	};
}