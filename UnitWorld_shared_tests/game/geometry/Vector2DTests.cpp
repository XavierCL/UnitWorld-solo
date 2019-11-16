#include "CppUnitTest.h"
#include "shared/game/geometry/Vector2D.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template <>
            static std::wstring ToString<Vector2D>(const Vector2D& v)
            {
                return std::wstring(L"{x:") + std::to_wstring(v.x()) + std::wstring(L", y:") + std::to_wstring(v.y()) + std::wstring(L"}");
            }
        }
    }
}

namespace UnitWorld_shared_tests
{
    TEST_CLASS(Vector2DTests)
    {
    public:

        TEST_METHOD(constructor_shouldInitializeAttributesTo0)
        {
            Vector2D testPoint;

            Assert::AreEqual(0.0, testPoint.x());
            Assert::AreEqual(0.0, testPoint.y());
        }

        TEST_METHOD(twoParametersConstructor_shouldInitializeBothXAndY)
        {
            const double ARBITRARY_X = 23.0;
            const double ARBITRARY_Y = 42.0;

            Vector2D testPoint(ARBITRARY_X, ARBITRARY_Y);

            Assert::AreEqual(ARBITRARY_X, testPoint.x());
            Assert::AreEqual(ARBITRARY_Y, testPoint.y());
        }

        TEST_METHOD(oneParametersConstructor_shouldInitializeBothXAndY)
        {
            const double ARBITRARY_X = 23.0;

            Vector2D testPoint(ARBITRARY_X);

            Assert::AreEqual(ARBITRARY_X, testPoint.x());
            Assert::AreEqual(0.0, testPoint.y());
        }

        TEST_METHOD(setX_shouldSaveTheXValueForLaterUse)
        {
            const double ARBITRARY_VALUE = 56.5;

            Vector2D testPoint;
            testPoint.x(ARBITRARY_VALUE);

            Assert::AreEqual(ARBITRARY_VALUE, testPoint.x());
        }

        TEST_METHOD(setY_shouldSaveTheYValueForLaterUse)
        {
            const double ARBITRARY_VALUE = 56.0;

            Vector2D testPoint;
            testPoint.y(ARBITRARY_VALUE);

            Assert::AreEqual(ARBITRARY_VALUE, testPoint.y());
        }

        TEST_METHOD(setXTheSecondTime_shouldSaveTheXValueForLaterUse)
        {
            const double ARBITRARY_VALUE = 56.4;
            const double SECOND_ARBITRARY_VALUE = 5241.2;

            Vector2D testPoint;
            testPoint.x(ARBITRARY_VALUE);
            testPoint.x(SECOND_ARBITRARY_VALUE);

            Assert::AreEqual(SECOND_ARBITRARY_VALUE, testPoint.x());
        }

        TEST_METHOD(setYTheSecondTime_shouldSaveTheYValueForLaterUse)
        {
            const double ARBITRARY_VALUE = 56;
            const double SECOND_ARBITRARY_VALUE = 5241;

            Vector2D testPoint;
            testPoint.y(ARBITRARY_VALUE);
            testPoint.y(SECOND_ARBITRARY_VALUE);

            Assert::AreEqual(SECOND_ARBITRARY_VALUE, testPoint.y());
        }

        TEST_METHOD(maxAt_shouldLeaveAValueAloneIfLowerThanMaxModule)
        {
            Vector2D testSpeed(1, 1);
            Vector2D expectedSpeed(testSpeed);

            testSpeed.maxAt(2);

            Assert::AreEqual(expectedSpeed, testSpeed);
        }

        TEST_METHOD(maxAt_shouldShortenAXDimensionedSpeedToTheGivenModule)
        {
            Vector2D testSpeed(4, 0);
            Vector2D expectedSpeed(3, 0);

            testSpeed.maxAt(3);

            Assert::AreEqual(expectedSpeed, testSpeed);
        }

        TEST_METHOD(maxAt_shouldShortenAYDimensionedSpeedToTheGivenModule)
        {
            Vector2D testSpeed(0, -4);
            Vector2D expectedSpeed(0, -3);

            testSpeed.maxAt(3);

            Assert::AreEqual(expectedSpeed, testSpeed);
        }

        TEST_METHOD(maxAt_shouldShortenABiDimensionedSpeedToTheGivenModule)
        {
            Vector2D testSpeed(-6, -8);
            Vector2D expectedSpeed(-3, -4);

            // 3^2 + 4^2 = 5^2
            testSpeed.maxAt(5);

            Assert::AreEqual(expectedSpeed, testSpeed);
        }
    };
}