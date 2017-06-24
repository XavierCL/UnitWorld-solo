#include "CppUnitTest.h"
#include "game/play/Unit.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace UnitWorld_shared_tests
{		
	TEST_CLASS(UnitTests)
	{
	public:
		
		TEST_METHOD(constructor_shouldInitializePositionTo0)
		{
			Unit testUnit;

			Assert::AreEqual(Point(0, 0).x(), testUnit.position().x());
			Assert::AreEqual(Point(0, 0).y(), testUnit.position().y());
		}

		TEST_METHOD(oneParameterConstructor_shouldSaveThePositionAsItsOwn)
		{
			const Point ARBITRARY_POINT(1, 1);
			Unit testUnit;

			testUnit.position(ARBITRARY_POINT);

			Assert::AreEqual(ARBITRARY_POINT.x(), testUnit.position().x());
			Assert::AreEqual(ARBITRARY_POINT.y(), testUnit.position().y());
		}

		TEST_METHOD(oneParameterConstructor_shouldSaveThePositionAsItsOwnOnSecondSet)
		{
			const Point ARBITRARY_POINT(1, 1);
			const Point SECOND_ARBITRARY_POINT(4, 5);
			Unit testUnit;

			testUnit.position(ARBITRARY_POINT);
			testUnit.position(SECOND_ARBITRARY_POINT);

			Assert::AreEqual(SECOND_ARBITRARY_POINT.x(), testUnit.position().x());
			Assert::AreEqual(SECOND_ARBITRARY_POINT.y(), testUnit.position().y());
		}
	};
}