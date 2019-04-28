#include "CppUnitTest.h"
#include "game/geometry/Rectangle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{
    TEST_CLASS(RectangleTests)
    {
    public:

        TEST_METHOD(threeParameterConstructor_shouldInitializePointsToRightValues)
        {
            Rectangle testRectangle(Vector2D(0, 1), Vector2D(2, 3));

            Assert::IsTrue(testRectangle.contains(Vector2D(0, 1)));
            Assert::IsTrue(testRectangle.contains(Vector2D(1, 1)));
            Assert::IsFalse(testRectangle.contains(Vector2D(-1, 1)));
            Assert::IsTrue(testRectangle.contains(Vector2D(0, 3)));
            Assert::IsFalse(testRectangle.contains(Vector2D(0, 4)));
            Assert::IsFalse(testRectangle.contains(Vector2D(0, 0)));
            Assert::IsTrue(testRectangle.contains(Vector2D(2, 1)));
            Assert::IsFalse(testRectangle.contains(Vector2D(3, 1)));
        }

        TEST_METHOD(threeParameterConstructor_shouldRemapTheGivenPointsToTheRightCorners)
        {
            Rectangle testRectangle(Vector2D(2, 1), Vector2D(0, 3));

            Assert::IsTrue(testRectangle.contains(Vector2D(0, 1)));
            Assert::IsTrue(testRectangle.contains(Vector2D(1, 1)));
            Assert::IsFalse(testRectangle.contains(Vector2D(-1, 1)));
            Assert::IsTrue(testRectangle.contains(Vector2D(0, 3)));
            Assert::IsFalse(testRectangle.contains(Vector2D(0, 4)));
            Assert::IsFalse(testRectangle.contains(Vector2D(0, 0)));
            Assert::IsTrue(testRectangle.contains(Vector2D(2, 1)));
            Assert::IsFalse(testRectangle.contains(Vector2D(3, 1)));
        }
    };
}