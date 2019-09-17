#include "CompleteGameStateMessage.h"

#include <nlohmann/json.hpp>

#include <chrono>

using namespace uw;

CompleteGameStateMessage::CompleteGameStateMessage(const CommunicatedCompleteGameState& completeGameState, const xg::Guid& currentPlayerId) :
    _completeGameState(completeGameState),
    _currentPlayerId(currentPlayerId)
{}

std::shared_ptr<CompleteGameStateMessage> CompleteGameStateMessage::fromJson(const std::string& json)
{
    nlohmann::json parsedData = nlohmann::json::parse(json);

    return std::make_shared<CompleteGameStateMessage>(
        CommunicatedCompleteGameState::fromJson(parsedData.at("complete-game-state")),
        xg::Guid(parsedData.at("currentPlayerId").get<std::string>())
    );
}

MessageType CompleteGameStateMessage::messageType() const
{
    return MessageType::CompleteGameStateMessageType;
}

std::string CompleteGameStateMessage::toJsonData() const
{
    nlohmann::json jsonData = {
        {"complete-game-state", _completeGameState.toJson()},
        {"currentPlayerId", _currentPlayerId.str()}
    };

    return jsonData.dump();
}

CommunicatedCompleteGameState CompleteGameStateMessage::completeGameState() const
{
    return _completeGameState;
}

xg::Guid CompleteGameStateMessage::getCurrentPlayerId() const
{
    return _currentPlayerId;
}