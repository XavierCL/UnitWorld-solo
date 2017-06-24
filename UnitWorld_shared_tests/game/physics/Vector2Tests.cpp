#include "CppUnitTest.h"
#include "game/physics/Vector2.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{		
	TEST_CLASS(Vector2Tests)
	{
	public:
		
		TEST_METHOD(constructor_shouldInitializeAttributesTo0)
		{
			Vector2 testPoint;

			Assert::AreEqual(0, testPoint.x());
			Assert::AreEqual(0, testPoint.y());
		}

		TEST_METHOD(twoParametersConstructor_shouldInitializeBothXAndY)
		{
			const int ARBITRARY_X = 23;
			const int ARBITRARY_Y = 42;

			Vector2 testPoint(ARBITRARY_X, ARBITRARY_Y);

			Assert::AreEqual(ARBITRARY_X, testPoint.x());
			Assert::AreEqual(ARBITRARY_Y, testPoint.y());
		}

		TEST_METHOD(oneParametersConstructor_shouldInitializeBothXAndY)
		{
			const int ARBITRARY_X = 23;

			Vector2 testPoint(ARBITRARY_X);

			Assert::AreEqual(ARBITRARY_X, testPoint.x());
			Assert::AreEqual(0, testPoint.y());
		}

		TEST_METHOD(setX_shouldSaveTheXValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;

			Vector2 testPoint;
			testPoint.x(ARBITRARY_VALUE);

			Assert::AreEqual(ARBITRARY_VALUE, testPoint.x());
		}

		TEST_METHOD(setY_shouldSaveTheYValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;

			Vector2 testPoint;
			testPoint.y(ARBITRARY_VALUE);

			Assert::AreEqual(ARBITRARY_VALUE, testPoint.y());
		}

		TEST_METHOD(setXTheSecondTime_shouldSaveTheXValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;
			const int SECOND_ARBITRARY_VALUE = 5241;

			Vector2 testPoint;
			testPoint.x(ARBITRARY_VALUE);
			testPoint.x(SECOND_ARBITRARY_VALUE);

			Assert::AreEqual(SECOND_ARBITRARY_VALUE, testPoint.x());
		}

		TEST_METHOD(setYTheSecondTime_shouldSaveTheYValueForLaterUse)
		{
			const int ARBITRARY_VALUE = 56;
			const int SECOND_ARBITRARY_VALUE = 5241;

			Vector2 testPoint;
			testPoint.y(ARBITRARY_VALUE);
			testPoint.y(SECOND_ARBITRARY_VALUE);

			Assert::AreEqual(SECOND_ARBITRARY_VALUE, testPoint.y());
		}
	};
}