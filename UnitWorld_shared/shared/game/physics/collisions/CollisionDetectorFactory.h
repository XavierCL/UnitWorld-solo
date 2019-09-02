#pragma once

#include "CollisionDetector.h"

namespace uw
{
    class CollisionDetectorFactory
    {
    public:
        virtual std::shared_ptr<CollisionDetector> create() = 0;
    };
}