#include "communications/ServerConnector.h"

#include "commons/Logger.hpp"

#include <thread>
#include <iostream>

void clientCallback(std::shared_ptr<CommunicationHandler> communicationHandler)
{
	Logger::info("Client " + communicationHandler->prettyName() + " connected");
}

int main()
{
	Logger::registerInfo([](const std::string& message) {
		std::cout << message;
	});
	Logger::info("Waiting for connections...\n");
	ServerConnector serverConnector(ConnectionInfo("127.0.0.1", "52124"), clientCallback);
	serverConnector.accept();
}
