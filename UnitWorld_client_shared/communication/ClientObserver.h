#pragma once

#include "../../UnitWorld_shared/sharedCommunication/ConnectionInfo.h"

class ClientObserver
{
public:
	ClientObserver(const ConnectionInfo& connectionInfo)
	{}

	virtual void gameStateReceived(GameState state) = 0;

	virtual void gameEnded
};
