#pragma once

#include "canvas/SFMLDrawingCanvas.h"
#include "units/GraphicalSinguity.h"

#include "clientgui/cameras/CameraRelativeGameManager.h"

#include "clientgui/userControls/UserControlState.h"

#include "shared/game/GameManager.h"

namespace uw
{
    class GameDrawer
    {
    public:
        GameDrawer(std::shared_ptr<GameManager> gameManager, std::shared_ptr<UserControlState> userControlState, std::shared_ptr<CameraRelativeGameManager> cameraRelativeGameManager):
            _gameManager(gameManager),
            _userControlState(userControlState),
            _cameraRelativeGameManager(cameraRelativeGameManager)
        {}

        void draw(std::shared_ptr<SFMLDrawingCanvas> canvas)
        {
            auto completeGameState = _gameManager->completeGameState();

            std::shared_ptr<std::unordered_map<xg::Guid, size_t>> playerIndexByPlayerId(std::make_shared<std::unordered_map<xg::Guid, size_t>>());
            for (size_t playerIndex = 0; playerIndex < completeGameState->players().size(); ++playerIndex)
            {
                (*playerIndexByPlayerId)[(completeGameState->players())[playerIndex]->id()] = playerIndex;
            }

            auto spawnersById = &completeGameState->spawners()
                | toUnorderedMap<xg::Guid, std::shared_ptr<Spawner>>([](std::shared_ptr<Spawner> spawner) { return spawner->id(); });

            const auto currentPlayerSinguities = _gameManager->currentPlayer()
                .map<std::shared_ptr<std::unordered_map<xg::Guid, std::shared_ptr<Singuity>>>>([](std::shared_ptr<Player> player) {
                    return player->singuities()
                        | toUnorderedMap<xg::Guid, std::shared_ptr<Singuity>>([](std::shared_ptr<Singuity> singuity) {
                        return singuity->id();
                    });
                }).getOrElse([] {return std::make_shared<std::unordered_map<xg::Guid, std::shared_ptr<Singuity>>>(); });

            _userControlState->getLastSelectedSpawnerId().foreach([&canvas, spawnersById, this](const xg::Guid selectedSpawnerId) {
                (spawnersById | find<std::shared_ptr<Spawner>>(selectedSpawnerId)).foreach([&canvas, this](std::shared_ptr<Spawner> spawner) {
                    const Circle spawnerCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                    const double spawnerSelectionRadius = spawnerCircle.radius() * 1.0454545454545454545454545454545;
                    sf::CircleShape graphicalSpawner(spawnerSelectionRadius);
                    graphicalSpawner.setPosition(round(spawnerCircle.center().x() - spawnerCircle.radius()), round(spawnerCircle.center().y() - spawnerCircle.radius()));
                    graphicalSpawner.setFillColor(sf::Color::White);
                    canvas->draw(graphicalSpawner);
                });
            });

            // Spawners
            &completeGameState->spawners() | forEach([&canvas, &playerIndexByPlayerId, this](std::shared_ptr<Spawner> spawner) {
                auto spanwerOuterAndInnerColors = spawner->allegence().map<std::pair<sf::Color, sf::Color>>([&spawner, &playerIndexByPlayerId, this](SpawnerAllegence allegence) {
                    auto playerFullColor = (playerIndexByPlayerId
                        | find<size_t>(allegence.allegedPlayerId()))
                        .map<sf::Color>([this](auto playerIndex) { return _playerColors[playerIndex]; })
                        .getOrElse(sf::Color::Black);

                    auto lifeRatio = allegence.healthPoint() / spawner->maximumHealthPoint();
                    sf::Color playerLifeColor(round(playerFullColor.r * lifeRatio), round(playerFullColor.g * lifeRatio), round(playerFullColor.b * lifeRatio));

                    return allegence.isClaimed()
                        ? std::make_pair(playerLifeColor, playerFullColor)
                        : std::make_pair(sf::Color(70, 70, 70), playerLifeColor);
                }).getOrElse(std::make_pair(sf::Color(70, 70, 70), sf::Color(70, 70, 70)));

                const Circle spawnerCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                const double spawnerInnerRadius = spawnerCircle.radius() * 0.90909090909090909090909090909091;
                sf::CircleShape graphicalSpawner(spawnerInnerRadius);
                graphicalSpawner.setPosition(round(spawnerCircle.center().x() - spawnerInnerRadius), round(spawnerCircle.center().y() - spawnerInnerRadius));
                graphicalSpawner.setFillColor(spanwerOuterAndInnerColors.first);
                graphicalSpawner.setOutlineThickness(spawnerCircle.radius() * 0.09090909090909090909090909090909);
                graphicalSpawner.setOutlineColor(spanwerOuterAndInnerColors.second);
                canvas->draw(graphicalSpawner);
            });
            
            // Last move units position
            _userControlState->getRelativeLastMoveUnitPosition().foreach([&canvas, this](const Vector2D& lastMoveUnitPosition) {
                const double circleRadius(_cameraRelativeGameManager->absoluteLengthToRelative(2.0));
                sf::CircleShape cursorDot(circleRadius);
                cursorDot.setPosition(round(lastMoveUnitPosition.x() - circleRadius), round(lastMoveUnitPosition.y() - circleRadius));
                cursorDot.setFillColor(sf::Color(255, 255, 255));
                canvas->draw(cursorDot);
            });

            // Selected unit white aura
            for (const auto selectedUnitId : _userControlState->getSelectedUnits())
            {
                (currentPlayerSinguities | find<std::shared_ptr<Singuity>>(selectedUnitId)).foreach([&canvas, this](std::shared_ptr<Singuity> singuity) {
                    Circle singuityCircle = _cameraRelativeGameManager->relativeCircleOf(singuity);
                    const double circleRadius(singuityCircle.radius() * 1.25);
                    sf::CircleShape selectedUnitAura(circleRadius);
                    selectedUnitAura.setPosition(round(singuityCircle.center().x() - circleRadius), round(singuityCircle.center().y() - circleRadius));
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
                    Circle singuityCircle = _cameraRelativeGameManager->relativeCircleOf(singuity);
                    canvas->draw(*(GraphicalSinguity(singuity, singuityCircle, drawingSinguitiesColor).drawable()));
                }
            }

            // User selection rectangle
            _userControlState->getRelativeSelectionRectangle().foreach([&canvas](const Rectangle& userSelection) {
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
        const std::shared_ptr<CameraRelativeGameManager> _cameraRelativeGameManager;
    };
}