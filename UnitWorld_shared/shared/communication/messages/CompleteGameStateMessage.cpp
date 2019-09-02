#include "CompleteGameStateMessage.h"

#include <nlohmann/json.hpp>

#include <chrono>

using namespace uw;

CompleteGameStateMessage::CompleteGameStateMessage(const CommunicatedCompleteGameState& completeGameState, const xg::Guid& currentPlayerId) :
    _completeGameState(completeGameState),
    _currentPlayerId(currentPlayerId)
{}

CompleteGameStateMessage::CompleteGameStateMessage(const std::string jsonData) :
    _completeGameState(jsonDataToCompleteGameState(jsonData)),
    _currentPlayerId(jsonDataToCurrentPlayerId(jsonData))
{}

MessageType CompleteGameStateMessage::messageType() const
{
    return MessageType::CompleteGameStateMessageType;
}

std::string CompleteGameStateMessage::toJsonData() const
{
    nlohmann::json jsonData = {
        {"complete-game-state", nlohmann::json::parse(_completeGameState.toJson())},
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

CommunicatedCompleteGameState CompleteGameStateMessage::jsonDataToCompleteGameState(const std::string& jsonData)
{
    nlohmann::json parsedData = nlohmann::json::parse(jsonData);

    return CommunicatedCompleteGameState::fromJson(parsedData.at("complete-game-state").dump());
}

xg::Guid CompleteGameStateMessage::jsonDataToCurrentPlayerId(const std::string& jsonData)
{
    nlohmann::json parsedData = nlohmann::json::parse(jsonData);

    return xg::Guid(parsedData.at("currentPlayerId").get<std::string>());
}