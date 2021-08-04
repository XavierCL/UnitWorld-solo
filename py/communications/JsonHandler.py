import json

class JsonHandler:
    @staticmethod
    def load(communication: str) -> dict:
        return json.loads(communication)

    @staticmethod
    def dump(message: dict) -> str:
        return json.dumps(message, separators=(',', ':'))
