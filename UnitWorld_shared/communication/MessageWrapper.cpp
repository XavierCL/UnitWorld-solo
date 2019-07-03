#include "MessageWrapper.h"

namespace uw
{
    NLOHMANN_JSON_SERIALIZE_ENUM(MessageType, {
        {InvalidMessageType, nullptr},
        {CompleteGameStateMessageType, "complete-game-state"},
    });

    const std::string MessageWrapper::MESSAGE_TYPE_JSON_ATTRIBUTE = "type";
    const std::string MessageWrapper::MESSAGE_DATA_JSON_ATTRIBUTE = "data";
}