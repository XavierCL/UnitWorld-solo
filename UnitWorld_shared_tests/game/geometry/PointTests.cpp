#include "CppUnitTest.h"
#include "game/geometry/Point.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{		
	TEST_CLASS(PointTests)
	{
	public:
		
		TEST_METHOD(constructor_shouldInitializeAttributesTo0)
		{
			Point testPoint;

			Assert::AreEqual(0, testPoint.x());
			Assert::AreEqual(0, testPoint.y());
		}

		TEST_METHOD(twoParametersConstructor_shouldInitializeBothXAndY)
		{
			const int ARBITRARY_X = 23;
			const int ARBITRARY_Y = 42;

			Point testPoint(ARBITRARY_X, ARBITRARY_Y);

			Assert::AreEqual(ARBITRARY_X, testPoint.x());
			Assert::AreEqual(ARBITRARY_Y, testPoint.y());
		}

		TEST_METHOD(oneParametersConstructor_shouldInitializeBothXAndY)
		{
			const int ARBITRARY_X = 23;

			Point testPoint(ARBITRARY_X);

			Assert::AreEqual(ARBITRARY_X, testPoint.x());
			Assert::AreEqual(0, testPoint.y());
		}

		TEST_METHOD(setX_shouldSaveTheXValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;

			Point testPoint;
			testPoint.x(ARBITRARY_VALUE);

			Assert::AreEqual(ARBITRARY_VALUE, testPoint.x());
		}

		TEST_METHOD(setY_shouldSaveTheYValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;

			Point testPoint;
			testPoint.y(ARBITRARY_VALUE);

			Assert::AreEqual(ARBITRARY_VALUE, testPoint.y());
		}

		TEST_METHOD(setXTheSecondTime_shouldSaveTheXValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;
			const int SECOND_ARBITRARY_VALUE = 5241;

			Point testPoint;
			testPoint.x(ARBITRARY_VALUE);
			testPoint.x(SECOND_ARBITRARY_VALUE);

			Assert::AreEqual(SECOND_ARBITRARY_VALUE, testPoint.x());
		}

		TEST_METHOD(setYTheSecondTime_shouldSaveTheYValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;
			const int SECOND_ARBITRARY_VALUE = 5241;

			Point testPoint;
			testPoint.y(ARBITRARY_VALUE);
			testPoint.y(SECOND_ARBITRARY_VALUE);

			Assert::AreEqual(SECOND_ARBITRARY_VALUE, testPoint.y());
		}
	};
}