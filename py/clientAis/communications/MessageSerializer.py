import json
from typing import List

from clientAis.communications.JsonHandler import JsonHandler

class MessageSerializer:
    def __init__(self):
        self.remainingDeserializationBuffer = ""

    @staticmethod
    def serialize(messages: List[dict]) -> str:
        return "\n".join([JsonHandler.dump(message) for message in messages])

    def deserialize(self, currentCommunication) -> List[dict]:
        self.remainingDeserializationBuffer += currentCommunication
        communications = self.remainingDeserializationBuffer.split("\\n")

        if len(communications[-1]) == 0:
            self.remainingDeserializationBuffer = ""
        else:
            self.remainingDeserializationBuffer = communications[-1]

        communications = communications[:-1]

        messages = []

        for communication in communications:
            try:
                messages.append(json.loads(communication))
            except:
                print(f"Error while parsing server message {communication[:min(100, len(communication))]}")

        return messages
