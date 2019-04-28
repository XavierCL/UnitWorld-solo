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

        TEST_METHOD(selectMobileUnitsInArea_shouldNotThrowAnyException)
        {
            Player testPlayer;
            testPlayer.selectMobileUnitsInArea(Rectangle(Vector2D(), Vector2D()));
        }

        TEST_METHOD(setSelectedMobileUnitsDestination_shouldNotThrowAnyException)
        {
            Player testPlayer;
            testPlayer.setSelectedMobileUnitsDestination(Vector2D());
        }
    };
}