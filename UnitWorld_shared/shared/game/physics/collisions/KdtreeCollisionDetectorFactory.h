#pragma once

#include "KdtreeCollisionDetector.h"

#include "CollisionDetectorFactory.h"

namespace uw
{
    class KdtreeCollisionDetectorFactory: public CollisionDetectorFactory
    {
    public:
        virtual std::shared_ptr<CollisionDetector> create()
        {
            return std::make_shared<KdtreeCollisionDetector>();
        }
    };
}