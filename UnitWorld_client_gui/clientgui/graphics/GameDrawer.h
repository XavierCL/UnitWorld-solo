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
            auto completeGameState = _gameManager->completeGameState();

            std::shared_ptr<std::unordered_map<xg::Guid, size_t>> playerIndexByPlayerId(std::make_shared<std::unordered_map<xg::Guid, size_t>>());
            for (size_t playerIndex = 0; playerIndex < completeGameState->players().size(); ++playerIndex)
            {
                (*playerIndexByPlayerId)[(completeGameState->players())[playerIndex]->id()] = playerIndex;
            }

            const auto currentPlayerSinguities = make_shared(completeGameState->players())
                | filter<Player>([this](auto player) { return player.id() == _currentPlayerId; })
                | flatMap<std::shared_ptr<Singuity>>([](auto player) { return player.singuities(); })
                | toUnorderedMap<xg::Guid, std::shared_ptr<const Singuity>>([](const std::shared_ptr<const Singuity>& singuity) {
                    return singuity->id();
                });

            // Spawners
            auto sharedSpawners(make_shared(completeGameState->spawners()));
            sharedSpawners | forEach([&canvas, &playerIndexByPlayerId, this](std::shared_ptr<Spawner> spawner) {
                auto spanwerOuterAndInnerColors = spawner->allegence().map<std::pair<sf::Color, sf::Color>>([&spawner, &playerIndexByPlayerId, this](SpawnerAllegence allegence) {
                    auto playerFullColor = (playerIndexByPlayerId
                        | find<size_t>(allegence.allegedPlayerId()))
                        .map<sf::Color>([this](auto playerIndex) { return _playerColors[playerIndex]; })
                        .getOrElse(sf::Color::Black);

                    auto lifeRatio = allegence.healthPoint() / spawner->maxHealthPoint();
                    sf::Color playerLifeColor(round(playerFullColor.r * lifeRatio), round(playerFullColor.g * lifeRatio), round(playerFullColor.b * lifeRatio));

                    return std::make_pair(playerFullColor, playerLifeColor);
                }).getOrElse(std::make_pair(sf::Color(70, 70, 70), sf::Color(70, 70, 70)));

                const int circleRadius(20.0);
                sf::CircleShape graphicalSpawner(circleRadius);
                graphicalSpawner.setPosition(round(spawner->position().x() - circleRadius), round(spawner->position().y() - circleRadius));
                graphicalSpawner.setFillColor(spanwerOuterAndInnerColors.second);
                graphicalSpawner.setOutlineThickness(2.0);
                graphicalSpawner.setOutlineColor(spanwerOuterAndInnerColors.first);
                canvas->draw(graphicalSpawner);
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
            for (auto player : completeGameState->players())
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
                sfUserSelection.setOutlineThickness(2.0);
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