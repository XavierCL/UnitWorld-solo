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
        CommunicatedCompleteGameState::fromJson(parsedData.at(COMPLETE_GAME_STATE_LABEL)),
        xg::Guid(parsedData.at(CURRENT_PLAYER_ID_LABEL).get<std::string>())
    );
}

MessageType CompleteGameStateMessage::messageType() const
{
    return MessageType::CompleteGameStateMessageType;
}

std::string CompleteGameStateMessage::toJsonData() const
{
    nlohmann::json jsonData = {
        {COMPLETE_GAME_STATE_LABEL, _completeGameState.toJson()},
        {CURRENT_PLAYER_ID_LABEL, _currentPlayerId.str()}
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

const std::string CompleteGameStateMessage::COMPLETE_GAME_STATE_LABEL = "c";
const std::string CompleteGameStateMessage::CURRENT_PLAYER_ID_LABEL = "u";