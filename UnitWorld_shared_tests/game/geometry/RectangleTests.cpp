#include "CppUnitTest.h"
#include "game/geometry/Rectangle.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{		
	TEST_CLASS(RectangleTests)
	{
	public:
		
		TEST_METHOD(constructor_shouldInitializePointTo0AndSizeTo0)
		{
			Rectangle testRectangle;

			Assert::IsFalse(testRectangle.contains(Point(0, 0)));
			Assert::IsFalse(testRectangle.contains(Point(1, 0)));
			Assert::IsFalse(testRectangle.contains(Point(-1, 0)));
			Assert::IsFalse(testRectangle.contains(Point(0, 1)));
			Assert::IsFalse(testRectangle.contains(Point(0, -1)));
		}

		TEST_METHOD(oneParameterConstructor_shouldInitializePointAndSizeTo0)
		{
			Rectangle testRectangle(Point(0, 1));

			Assert::IsFalse(testRectangle.contains(Point(0, 1)));
			Assert::IsFalse(testRectangle.contains(Point(1, 1)));
			Assert::IsFalse(testRectangle.contains(Point(-1, 1)));
			Assert::IsFalse(testRectangle.contains(Point(0, 2)));
			Assert::IsFalse(testRectangle.contains(Point(0, 0)));
		}

		TEST_METHOD(twoParameterConstructor_shouldInitializePointAndSizeTo0)
		{
			Rectangle testRectangle(Point(0, 1), 2);

			Assert::IsFalse(testRectangle.contains(Point(0, 1)));
			Assert::IsFalse(testRectangle.contains(Point(1, 1)));
			Assert::IsFalse(testRectangle.contains(Point(-1, 1)));
			Assert::IsFalse(testRectangle.contains(Point(0, 2)));
			Assert::IsFalse(testRectangle.contains(Point(0, 0)));
			Assert::IsFalse(testRectangle.contains(Point(2, 1)));
		}

		TEST_METHOD(threeParameterConstructor_shouldInitializePointAndSizeTo0)
		{
			Rectangle testRectangle(Point(0, 1), 2, 3);

			Assert::IsTrue(testRectangle.contains(Point(0, 1)));
			Assert::IsTrue(testRectangle.contains(Point(1, 1)));
			Assert::IsFalse(testRectangle.contains(Point(-1, 1)));
			Assert::IsTrue(testRectangle.contains(Point(0, 3)));
			Assert::IsFalse(testRectangle.contains(Point(0, 4)));
			Assert::IsFalse(testRectangle.contains(Point(0, 0)));
			Assert::IsFalse(testRectangle.contains(Point(2, 1)));
		}
	};
}