#include "communication/ServerConnector.h"

#include "../UnitWorld_shared/utils/Logger.h"

void clientCallback(std::shared_ptr<CommunicationHandler> communicationHandler)
{
	Logger::info("Client" + communicationHandler->prettyName());
}

int main()
{
	Logger::registerInfo([](const std::string& message) {
		OutputDebugStringA(message.data());
	});
	ServerConnector serverConnector(ConnectionInfo("127.0.0.1", "52124"), clientCallback);
}