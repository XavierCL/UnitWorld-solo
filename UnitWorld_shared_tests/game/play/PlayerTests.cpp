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
            Player testPlayer(std::vector<std::shared_ptr<Singuity>>());
        }

        TEST_METHOD(selectMobileUnitsInArea_shouldNotThrowAnyException)
        {
            const std::vector<std::shared_ptr<Singuity>> noSinguities;
            Player testPlayer(noSinguities);
            testPlayer.selectMobileUnitsInArea(Rectangle(Vector2D(), Vector2D()));
        }

        TEST_METHOD(setSelectedMobileUnitsDestination_shouldNotThrowAnyException)
        {
            const std::vector<std::shared_ptr<Singuity>> noSinguities;
            Player testPlayer(noSinguities);
            testPlayer.setSelectedMobileUnitsDestination(Vector2D());
        }
    };
}