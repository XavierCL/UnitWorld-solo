#pragma once

#include "canvas/SFMLDrawingCanvas.h"
#include "units/GraphicalSinguity.h"

#include "clientgui/userControls/UserControlState.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class GameDrawer
    {
    public:
        GameDrawer(std::shared_ptr<GameManager> gameManager, std::shared_ptr<UserControlState> userControlState, const xg::Guid& currentPlayerId):
            _gameManager(gameManager),
            _userControlState(userControlState),
            _currentPlayerId(currentPlayerId)
        {}

        void draw(std::shared_ptr<SFMLDrawingCanvas> canvas)
        {
            const auto localPlayers = std::make_shared<immer::vector<std::shared_ptr<Player>>>(_gameManager->players());

            std::shared_ptr<std::unordered_map<xg::Guid, size_t>> playerIndexByPlayerId(std::make_shared<std::unordered_map<xg::Guid, size_t>>());
            for (size_t playerIndex = 0; playerIndex < localPlayers->size(); ++playerIndex)
            {
                (*playerIndexByPlayerId)[(*localPlayers)[playerIndex]->id()] = playerIndex;
            }

            const auto currentPlayerSinguities = localPlayers
                | filter<std::shared_ptr<Player>>([this](auto player) { return player->id() == _currentPlayerId; })
                | flatMap<std::shared_ptr<Singuity>>([](auto player) { return player->singuities(); })
                | toUnorderedMap<xg::Guid, std::shared_ptr<const Singuity>>([](const std::shared_ptr<const Singuity>& singuity) {
                    return singuity->id();
                });

            // Last move units position
            _userControlState->getLastMoveUnitPosition().foreach([&canvas](const Vector2D& lastMoveUnitPosition) {
                const int circleRadius(2.0);
                sf::CircleShape cursorDot(circleRadius);
                cursorDot.setPosition(round(lastMoveUnitPosition.x() - circleRadius), round(lastMoveUnitPosition.y() - circleRadius));
                cursorDot.setFillColor(sf::Color(255, 255, 255));
                canvas->draw(cursorDot);
            });

            // Selected unit white aura
            for (const auto selectedUnitId : _userControlState->getSelectedUnits())
            {
                (currentPlayerSinguities | find<std::shared_ptr<const Singuity>>(selectedUnitId)).foreach([&canvas](const std::shared_ptr<const Singuity>& singuity) {
                    const int circleRadius(5.0);
                    sf::CircleShape selectedUnitAura(circleRadius);
                    selectedUnitAura.setPosition(round(singuity->position().x() - circleRadius), round(singuity->position().y() - circleRadius));
                    selectedUnitAura.setFillColor(sf::Color(255, 255, 255));
                    canvas->draw(selectedUnitAura);
                });
            }

            // Units
            for (auto player : *localPlayers)
            {
                sf::Color drawingSinguitiesColor = (playerIndexByPlayerId
                    | find<size_t>(player->id()))
                    .map<sf::Color>([this](auto playerIndex) { return _playerColors[playerIndex]; })
                    .getOrElse(sf::Color::Black);

                for (auto singuity : *player->singuities())
                {
                    canvas->draw(*(GraphicalSinguity(*singuity, drawingSinguitiesColor).drawable()));
                }
            }

            // User selection rectangle
            _userControlState->getSelectionRectangle().foreach([&canvas](const Rectangle& userSelection) {
                const auto userSelectionSize(userSelection.size());
                const auto userSelectionLowerRight(userSelection.upperLeftCorner());
                sf::RectangleShape sfUserSelection(sf::Vector2f(userSelectionSize.x(), userSelectionSize.y()));
                sfUserSelection.setPosition(userSelectionLowerRight.x(), userSelectionLowerRight.y());
                sfUserSelection.setFillColor(sf::Color::Transparent);
                sfUserSelection.setOutlineThickness(1);
                canvas->draw(sfUserSelection);
            });
        }

    private:

        static const std::vector<sf::Color> _playerColors;

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<UserControlState> _userControlState;
        const xg::Guid _currentPlayerId;
    };
}