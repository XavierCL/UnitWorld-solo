#include "CppUnitTest.h"
#include "shared/game/play/players/Player.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{
    TEST_CLASS(PlayerTests)
    {
    public:

        TEST_METHOD(constructor_shouldNotThrowAnyException)
        {
            Player(xg::newGuid(), std::vector<std::shared_ptr<Singuity>>());
        }
    };
}