#pragma once

#include "CollisionDetectorFactory.h"

#include "NaiveCollisionDetector.h"

namespace uw
{
    class NaiveCollisionDetectorFactory: public CollisionDetectorFactory
    {
    public:
        std::shared_ptr<CollisionDetector> create()
        {
            return std::make_shared<NaiveCollisionDetector>();
        }
    };
}