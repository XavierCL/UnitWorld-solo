#include "CppUnitTest.h"
#include "shared/game/physics/collisions/KdtreeCollisionDetector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace uw;

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            template <>
            static std::wstring ToString<CollidablePoint>(const CollidablePoint& v)
            {
                const auto vAsciiString = "{id:" + v.id().str() + ", x:" + std::to_string(v.position().x()) + ", y:" + std::to_string(v.position().y()) = "}";
                return std::wstring(vAsciiString.begin(), vAsciiString.end());
            }

            template <>
            static std::wstring ToString<xg::Guid>(const xg::Guid& v)
            {
                const auto vAsciiString = v.str();
                return std::wstring(vAsciiString.begin(), vAsciiString.end());
            }
        }
    }
}

namespace UnitWorld_shared_tests
{
    TEST_CLASS(KdtreeCollisionDetectorTests)
    {
    public:

        TEST_METHOD_INITIALIZE(Setup)
        {
            _collisionDetector.updateAllCollidablePoints(std::vector<CollidablePoint> {});
        }

        TEST_METHOD(given_no_point_when_getting_closest_then_no_point_is_returned)
        {
            _collisionDetector.updateAllCollidablePoints(std::vector<CollidablePoint>{});

            const auto closestPointId = _collisionDetector.getClosest(CollidablePoint(xg::newGuid(), Vector2D(300, 11)));

            Assert::IsTrue(closestPointId.isEmpty());
        }

        TEST_METHOD(given_a_single_point_when_getting_closest_then_the_single_point_is)
        {
            CollidablePoint singlePoint(xg::newGuid(), Vector2D(403, 232));
            _collisionDetector.updateAllCollidablePoints(std::vector<CollidablePoint>{ singlePoint });

            const auto closestPointId = _collisionDetector.getClosest(CollidablePoint(xg::newGuid(), Vector2D(300, 11)));

            Assert::AreEqual(closestPointId.getOrThrow(), singlePoint.id());
        }

        TEST_METHOD(given_a_point_and_the_searched_then_getting_closest_then_the_other_point_is)
        {
            CollidablePoint searched(xg::newGuid(), Vector2D(4062, 22));
            CollidablePoint otherPoint(xg::newGuid(), Vector2D(403, 232));
            _collisionDetector.updateAllCollidablePoints(std::vector<CollidablePoint>{ searched, otherPoint });

            const auto closestPointId = _collisionDetector.getClosest(searched);

            Assert::AreEqual(closestPointId.getOrThrow(), otherPoint.id());
        }

        TEST_METHOD(given_three_points_gradually_farther_from_origin_when_getting_closest_then_all_closest_are_correct)
        {
            CollidablePoint first(xg::newGuid(), Vector2D(0, 0));
            CollidablePoint middle(xg::newGuid(), Vector2D(10, 10));
            CollidablePoint farthest(xg::newGuid(), Vector2D(21, 21));
            _collisionDetector.updateAllCollidablePoints(std::vector<CollidablePoint> {farthest, middle, first});

            const auto firstClosest = _collisionDetector.getClosest(first);
            Assert::AreEqual(firstClosest.getOrThrow(), middle.id());

            const auto middleClosest = _collisionDetector.getClosest(middle);
            Assert::AreEqual(middleClosest.getOrThrow(), first.id());

            const auto farthestClosest = _collisionDetector.getClosest(farthest);
            Assert::AreEqual(farthestClosest.getOrThrow(), middle.id());
        }

        TEST_METHOD(given_four_points_in_an_irregular_square_when_getting_closest_then_all_closest_are_correct)
        {
            CollidablePoint first(xg::newGuid(), Vector2D(-100, -100));
            CollidablePoint second(xg::newGuid(), Vector2D(-100, 99));
            CollidablePoint third(xg::newGuid(), Vector2D(98, 99));
            CollidablePoint fourth(xg::newGuid(), Vector2D(98, -80));
            _collisionDetector.updateAllCollidablePoints(std::vector<CollidablePoint> {first, third, second, fourth});

            const auto firstClosest = _collisionDetector.getClosest(first);
            Assert::AreEqual(firstClosest.getOrThrow(), second.id());

            const auto secondClosest = _collisionDetector.getClosest(second);
            Assert::AreEqual(secondClosest.getOrThrow(), third.id());

            const auto thirdClosest = _collisionDetector.getClosest(third);
            Assert::AreEqual(thirdClosest.getOrThrow(), fourth.id());

            const auto fourthClosest = _collisionDetector.getClosest(fourth);
            Assert::AreEqual(fourthClosest.getOrThrow(), third.id());
        }

    private:
        KdtreeCollisionDetector _collisionDetector;
    };
}