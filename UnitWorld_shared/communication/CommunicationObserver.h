#pragma once

#include "CommunicationHandler.h"

class CommunicationObserver
{
public:
	CommunicationObserver(const std::shared_ptr<CommunicationHandler>& communicationHandler):
		_communicationHandler(communicationHandler)
	{}

private:
	std::shared_ptr<CommunicationHandler> _communicationHandler;
};