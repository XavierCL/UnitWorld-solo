#include "SetSpawnersRallyMessage.h"

#include <nlohmann/json.hpp>

using namespace uw;

SetSpawnersRallyMessage::SetSpawnersRallyMessage(const std::vector<xg::Guid>& spawnersId, const CommunicatedSinguityDestination& destination) :
    _spawnersId(spawnersId),
    _destination(destination)
{}

std::shared_ptr<SetSpawnersRallyMessage> SetSpawnersRallyMessage::fromJson(const std::string& json)
{
    nlohmann::json parsedData = nlohmann::json::parse(json);

    std::vector<xg::Guid> spawnersId;
    for (const auto spawnerId : parsedData.at(SPAWNERS_ID_LABEL))
    {
        spawnersId.emplace_back(spawnerId.get<std::string>());
    }

    return std::make_shared<SetSpawnersRallyMessage>(
        std::move(spawnersId),
        CommunicatedSinguityDestination::fromJson(parsedData.at(DESTINATION_LABEL))
    );
}

MessageType SetSpawnersRallyMessage::messageType() const
{
    return MessageType::SetSpawnersRallyMessageType;
}

std::string SetSpawnersRallyMessage::toJsonData() const
{
    std::vector<std::string> spawnersStringId;
    for (const auto spawnerId : _spawnersId)
    {
        spawnersStringId.emplace_back(spawnerId.str());
    }
    nlohmann::json returnedJson({
        {SPAWNERS_ID_LABEL, spawnersStringId},
        {DESTINATION_LABEL, _destination.toJson()}
    });

    return returnedJson.dump();
}

std::vector<xg::Guid> SetSpawnersRallyMessage::spawnersId() const
{
    return _spawnersId;
}

CommunicatedSinguityDestination SetSpawnersRallyMessage::destination() const
{
    return _destination;
}

const std::string SetSpawnersRallyMessage::SPAWNERS_ID_LABEL = "s";
const std::string SetSpawnersRallyMessage::DESTINATION_LABEL = "d";