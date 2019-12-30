#pragma once

#include "ImmutableClientGameStateFactory.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class ClientGameStateFactory
    {
    public:
        ClientGameStateFactory(bool isFogOfWarEnabled, const std::shared_ptr<CollisionDetectorFactory>& collisionDetectorFactory):
            _isFogOfWarEnabled(isFogOfWarEnabled),
            _collisionDetectorFactory(collisionDetectorFactory)
        {}

        ImmutableClientGameStateFactory createImmutableStateFactory(const std::shared_ptr<const CompleteGameState> completeGameState)
        {
            return ImmutableClientGameStateFactory(completeGameState, _isFogOfWarEnabled, _collisionDetectorFactory);
        }

    private:

        const bool _isFogOfWarEnabled;
        const std::shared_ptr<CollisionDetectorFactory> _collisionDetectorFactory;
    };
}