#include "AiStats.h"

#include "shared/configuration/ConfigurationManager.h"

#include "communications/ClientConnector.h"

#include "commons/Logger.hpp"

#include "ais/Artificial.h"

#include "ais/xaviercl/xavierclAi1/XavierclAi1.h"
#include "ais/xaviercl/gotoClosestSpawner/GotoClosestSpawner.h"
#include "ais/xaviercl/packAi/PackAi.h"
#include "ais/xaviercl/strongholdAi/StrongholdAi.h"
#include "ais/xaviercl/strongholdAi2/StrongholdAi2.h"
#include "ais/xaviercl/arcAi/ArcAi.h"
#include "ais/xaviercl/spearAi/SpearAi.h"

#include "ais/johnwl/microer/Microer.h"
#include "ais/johnwl/microer/Microer2.h"

#include "clientShared/networking/ServerReceiver.h"

#include "shared/game/physics/collisions/KdtreeCollisionDetectorFactory.h"

#include "shared/game/physics/PhysicsManager.h"

#include <chrono>
#include <fstream>

using namespace uw;

std::shared_ptr<Artificial> generateAI(const std::string& aiName)
{
    Logger::info("Using Ai with name: " + aiName);
    if (aiName == "xavierclAi1")
    {
        return std::make_shared<XavierclAi1>();
    }
    else if (aiName == "gotoClosestSpawner")
    {
        return std::make_shared<GotoClosestSpawner>();
    }
    else if (aiName == "packAi")
    {
        return std::make_shared<PackAi>();
    }
    else if (aiName == "strongholdAi")
    {
        return std::make_shared<StrongholdAi>();
    }
    else if (aiName == "strongholdAi2")
    {
        return std::make_shared<StrongholdAi2>();
    }
    else if (aiName == "arcAi")
    {
        return std::make_shared<ArcAi>();
    }
    else if (aiName == "microer")
    {
        return std::make_shared<Microer>();
    }
    else if (aiName == "microer2")
    {
        return std::make_shared<Microer2>();
    }
    else if (aiName == "spearAi")
    {
        return std::make_shared<SpearAi>();
    }
    else
    {
        Logger::error("Could not find ai with name: " + aiName + ". Default ai will be used");
        return std::make_shared<GotoClosestSpawner>();
    }
}

int main()
{
    const ConfigurationManager configurationManager("config.json");
    const std::string DEFAULT_SERVER_IP("127.0.0.1");
    const std::string DEFAULT_SERVER_PORT("52124");
    const std::string DEFAULT_AI_NAME("gotoClosestSpawner");
    const double MS_PER_FRAME(250);

    const std::string serverIp = configurationManager.serverIpOrDefault(DEFAULT_SERVER_IP);
    const std::string serverPort = configurationManager.serverPortOrDefault(DEFAULT_SERVER_PORT);
    const std::string aiName = configurationManager.aiName(DEFAULT_AI_NAME);

    std::ofstream traceLogFile("trace_client_ai_" + aiName + ".log", std::ios_base::app);
    Logger::registerError([](const std::string& errorMessage) {
        std::cout << "ERROR: " << errorMessage << std::endl;
    });
    Logger::registerInfo([](const std::string& message) {
        std::cout << "INFO: " << message << std::endl;
    });
    Logger::registerTrace([&traceLogFile](const auto& traceMessage) {

        auto currentTime = std::time(nullptr);
        auto localTime = *std::localtime(&currentTime);

        traceLogFile << std::put_time(&localTime, "%Y-%m-%dT%H-%M-%S") << ": " << traceMessage << std::endl;
        traceLogFile.flush();
    });

    const std::shared_ptr<Artificial> someAI = generateAI(aiName);

    ClientConnector(ConnectionInfo(serverIp, serverPort), [someAI, MS_PER_FRAME](const std::shared_ptr<CommunicationHandler>& connectionHandler) {

        const auto gameManager(std::make_shared<GameManager>());

        const auto physicsCommunicationAssembler(std::make_shared<PhysicsCommunicationAssembler>());
        const auto messageSerializer(std::make_shared<MessageSerializer>());
        const auto serverCommander(std::make_shared<ServerCommander>(connectionHandler, physicsCommunicationAssembler, messageSerializer));

        const auto serverReceiver(std::make_shared<ServerReceiver>(connectionHandler, gameManager, physicsCommunicationAssembler, messageSerializer));

        const auto kdtreeCollisionDetectorFactory(std::make_shared<KdtreeCollisionDetectorFactory>());
        const auto physicsManager(std::make_shared<PhysicsManager>(gameManager, kdtreeCollisionDetectorFactory, std::make_shared<PhysicsStats>()));

        const auto aiStats(std::make_shared<AiStats>());

        serverReceiver->startAsync();
        physicsManager->startAsync();

        Logger::info("Started game evaluation");

        while (connectionHandler->isOpen())
        {
            const auto startFrameTime = std::chrono::steady_clock::now();

            someAI->frameHappened(gameManager, serverCommander);

            const auto endFrameTime = std::chrono::steady_clock::now();

            const auto frameTimeInMs = (unsigned int)std::chrono::duration<double, std::milli>(endFrameTime - startFrameTime).count();
            aiStats->feedFrameDuration(frameTimeInMs);

            if (frameTimeInMs < MS_PER_FRAME)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds((long)MS_PER_FRAME - frameTimeInMs));
            }
        }

        serverReceiver->stop();
        physicsManager->stop();

    }, [](const std::error_code& errorCode) {
        Logger::error("Could not connect to the server. Error code #" + std::to_string(errorCode.value()) + ". Error message: " + errorCode.message());
    });
    return 0;
}