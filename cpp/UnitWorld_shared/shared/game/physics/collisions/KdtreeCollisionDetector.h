#pragma once

#include "CollisionDetector.h"

#include "commons/Option.hpp"

namespace uw
{
    class KdtreeCollisionDetector: public CollisionDetector
    {
    private:
        struct KdtreeNode
        {
            KdtreeNode(CollidablePoint* points, size_t length, size_t depth)
            {
                if (length == 1)
                {
                    _current = points;
                }
                else if (length == 2)
                {
                    bool isXComparison = depth % 2 == 0;
                    if (isXComparison && points[0].position().x() <= points[1].position().y() || !isXComparison && points[0].position().y() <= points[1].position().y())
                    {
                        _current = &points[1];
                        _lowests = Options::Some(KdtreeNode(points, 1, depth + 1));
                    }
                    else
                    {
                        _current = &points[0];
                        _lowests = Options::Some(KdtreeNode(points + 1, 1, depth + 1));
                    }
                }
                else
                {
                    bool isXComparison = depth % 2 == 0;

                    if (isXComparison)
                    {
                        placeNthElement(points, length, length / 2, KdtreeNode::xDepthFirstSmallerThanSecond);
                    }
                    else
                    {
                        placeNthElement(points, length, length / 2, KdtreeNode::yDepthFirstSmallerThanSecond);
                    }

                    size_t median = length / 2;
                    _current = &points[median];
                    _lowests = Options::Some(KdtreeNode(points, median, depth + 1));
                    _greatests = Options::Some(KdtreeNode(points + median + 1, (length - 1) / 2, depth + 1));
                }
            }

            CollidablePoint* getClosest(const CollidablePoint& target, size_t depth, CollidablePoint* currentBest) const
            {
                bool isXComparison = depth % 2 == 0;
                if (_current->id() != target.id())
                {
                    currentBest = bestCollidable(target, currentBest, _current);
                }

                if (_greatests.isDefined() && ((isXComparison && _current->position().x() <= target.position().x())
                    || (!isXComparison && _current->position().y() <= target.position().y())))
                {
                    currentBest = bestCollidable(target, currentBest, _greatests.getOrThrow().getClosest(target, depth + 1, currentBest));
                    if (!currentBest || (isXComparison && currentBest->position().distanceSq(target.position()) > sq(target.position().x() - _current->position().x()))
                        || (!isXComparison && currentBest->position().distanceSq(target.position()) > sq(target.position().y() - _current->position().y())))
                    {
                        currentBest = bestCollidable(target, currentBest, _lowests.getOrThrow().getClosest(target, depth + 1, currentBest));
                    }
                }
                else if (_lowests.isDefined())
                {
                    currentBest = bestCollidable(target, currentBest, _lowests.getOrThrow().getClosest(target, depth + 1, currentBest));
                    if (_greatests.isDefined() && (!currentBest || (isXComparison && currentBest->position().distanceSq(target.position()) > sq(_current->position().x() - target.position().x()))
                        || (!isXComparison && currentBest->position().distanceSq(target.position()) > sq(_current->position().y() - target.position().y()))))
                    {
                        currentBest = bestCollidable(target, currentBest, _greatests.getOrThrow().getClosest(target, depth + 1, currentBest));
                    }
                }
                return currentBest;
            }

            static CollidablePoint* bestCollidable(const CollidablePoint& target, CollidablePoint* oldBest, CollidablePoint* hopeBest)
            {
                if (oldBest)
                {
                    auto bestDist = target.position().distanceSq(oldBest->position());
                    auto currentBestDist = target.position().distanceSq(hopeBest->position());
                    return currentBestDist < bestDist ? hopeBest : oldBest;
                }
                else
                {
                    return hopeBest;
                }
            }

            static double sq(const double& v)
            {
                return v * v;
            }

            static bool xDepthFirstSmallerThanSecond(const CollidablePoint& first, const CollidablePoint& second)
            {
                return first.position().x() < second.position().x();
            }

            static bool yDepthFirstSmallerThanSecond(const CollidablePoint& first, const CollidablePoint& second)
            {
                return first.position().y() < second.position().y();
            }

            template <typename Point, typename FirstSmallerThanSecond>
            static void placeNthElement(Point* points, const size_t& length, const size_t& target, const FirstSmallerThanSecond& firstSmallerThanSecond)
            {
                std::swap(points[target], points[length - 1]);

                size_t selectionIndex = 0;
                size_t placementIndex = 0;
                while (selectionIndex < length - 1)
                {
                    if (firstSmallerThanSecond(points[selectionIndex], points[length - 1]))
                    {
                        std::swap(points[selectionIndex], points[placementIndex]);
                        ++placementIndex;
                    }
                    ++selectionIndex;
                }
                std::swap(points[length - 1], points[placementIndex]);

                if (placementIndex > target)
                {
                    placeNthElement(points, placementIndex, target, firstSmallerThanSecond);
                }
                else if (placementIndex < target)
                {
                    placeNthElement(points + placementIndex + 1, length - (placementIndex + 1), target - (placementIndex + 1), firstSmallerThanSecond);
                }
            }

            CollidablePoint* _current;
            Option<KdtreeNode> _lowests;
            Option<KdtreeNode> _greatests;
        };

    public:
        virtual void updateAllCollidablePoints(const std::vector<CollidablePoint>& collidablePoints)
        {
            _copy = collidablePoints;
            if (_copy.size() < 1)
            {
                _root = Options::None<KdtreeNode>();
            }
            else
            {
                _root = Options::Some(KdtreeNode(_copy.data(), _copy.size(), 0));
            }
        }

        virtual Option<xg::Guid> getClosest(const CollidablePoint& target)
        {
            return _root.flatMap<xg::Guid>([&target](const KdtreeNode& node) {
                auto* closest = node.getClosest(target, 0, nullptr);
                return closest != nullptr ? Options::Some(closest->id()) : Options::None<xg::Guid>();
            });
        }

    private:

        std::vector<CollidablePoint> _copy;
        Option<KdtreeNode> _root;
    };
}