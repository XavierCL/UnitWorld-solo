#pragma once

namespace uw
{
    enum MessageType
    {
        InvalidMessageType,
        CompleteGameStateMessageType,
        MoveMobileUnitsToPositionMessageType,
        MoveMobileUnitsToSpawnerMessageType,
        SetSpawnersRallyMessageType
    };
}