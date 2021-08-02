#pragma once

#include "canvas/SFMLDrawingCanvas.h"

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
            _cameraRelativeGameManager(cameraRelativeGameManager),
            _lastDrawnGameStateVersion()
        {}

        void draw(std::shared_ptr<SFMLDrawingCanvas> canvas)
        {
            auto completeGameState = _gameManager->completeGameState();

            _lastDrawnGameStateVersion = completeGameState->version();

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

            // Selected unit white aura
            sf::CircleShape selectedUnitCircleShape;
            for (const auto selectedUnitId : _userControlState->getSelectedUnits())
            {
                (currentPlayerSinguities | findSafe<std::shared_ptr<Singuity>>(selectedUnitId)).foreach([&selectedUnitCircleShape, &canvas, this](std::shared_ptr<Singuity> singuity) {
                    Circle singuityCircle = _cameraRelativeGameManager->relativeCircleOf(singuity);
                    const double circleRadius(singuityCircle.radius() * 1.25);

                    setCircleProperties(selectedUnitCircleShape, circleRadius, singuityCircle.center(), sf::Color::White, 0.0, sf::Color::White);

                    canvas->draw(selectedUnitCircleShape);
                });
            }

            // Units
            for (auto player : completeGameState->players())
            {
                sf::Color drawingSinguitiesColor = (playerIndexByPlayerId
                    | find<size_t>(player->id()))
                    .map<sf::Color>([this](auto playerIndex) { return _playerColors[playerIndex]; })
                    .getOrElse(sf::Color::Black);

                auto playerSinguities = player->singuities();

                sf::CircleShape circleShape;
                for (size_t index = 0; index < playerSinguities->size(); ++index)
                {
                    auto singuity = playerSinguities->operator[](index);
                    Circle singuityCircle = _cameraRelativeGameManager->relativeCircleOf(singuity);

                    auto lifeRatio = singuity->healthPoint() / singuity->maximumHealthPoint();
                    sf::Color lifeColor(round(drawingSinguitiesColor.r * lifeRatio), round(drawingSinguitiesColor.g * lifeRatio), round(drawingSinguitiesColor.b * lifeRatio));

                    const auto innerSinguityRadius(singuityCircle.radius() * 0.75);

                    setCircleProperties(circleShape, innerSinguityRadius, singuityCircle.center(), lifeColor, singuityCircle.radius() * 0.25, drawingSinguitiesColor);

                    canvas->draw(circleShape);
                }
            }

            // Target spawnerId aura
            _userControlState->getLastSelectedSpawnerId().foreach([&canvas, spawnersById, this](const xg::Guid selectedSpawnerId) {
                (spawnersById | find<std::shared_ptr<Spawner>>(selectedSpawnerId)).foreach([&canvas, this](std::shared_ptr<Spawner> spawner) {
                    const Circle spawnerCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                    const double spawnerSelectionRadius = spawnerCircle.radius() * 1.0454545454545454545454545454545;
                    sf::CircleShape graphicalSpawner(spawnerSelectionRadius);
                    graphicalSpawner.setPosition(spawnerCircle.center().x() - spawnerSelectionRadius, spawnerCircle.center().y() - spawnerSelectionRadius);
                    graphicalSpawner.setFillColor(sf::Color::White);
                    canvas->draw(graphicalSpawner);
                });
            });

            // Selected spawnersId aura
            for (const auto& selectedSpawnerId : _userControlState->getSelectedSpawners())
            {
                (spawnersById | find<std::shared_ptr<Spawner>>(selectedSpawnerId)).foreach([&canvas, this](std::shared_ptr<Spawner> spawner) {
                    const Circle spawnerCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                    const double spawnerSelectionRadius = spawnerCircle.radius() * 1.0454545454545454545454545454545;
                    sf::CircleShape graphicalSpawner(spawnerSelectionRadius);
                    graphicalSpawner.setPosition(spawnerCircle.center().x() - spawnerSelectionRadius, spawnerCircle.center().y() - spawnerSelectionRadius);
                    graphicalSpawner.setFillColor(sf::Color::White);
                    canvas->draw(graphicalSpawner);
                });
            }

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
                graphicalSpawner.setPosition(spawnerCircle.center().x() - spawnerInnerRadius, spawnerCircle.center().y() - spawnerInnerRadius);
                graphicalSpawner.setFillColor(spanwerOuterAndInnerColors.first);
                graphicalSpawner.setOutlineThickness(spawnerCircle.radius() * 0.09090909090909090909090909090909);
                graphicalSpawner.setOutlineColor(spanwerOuterAndInnerColors.second);
                canvas->draw(graphicalSpawner);
            });

            // Spawner rallies
            for (const auto& selectedSpawnerId : _userControlState->getSelectedSpawners())
            {
                (spawnersById | find<std::shared_ptr<Spawner>>(selectedSpawnerId)).foreach([&canvas, this, &spawnersById](std::shared_ptr<Spawner> selectedSpawner) {
                    Option<Vector2D> rallyPosition = selectedSpawner->rally().flatMap<Vector2D>([&spawnersById](const MobileUnitDestination& destination) {
                        return destination.map<Option<Vector2D>>(
                            [](const auto& positionDestination) { return Options::Some(positionDestination); },
                            [&spawnersById](const SpawnerDestination& spawnerDestination) { return (spawnersById | find<std::shared_ptr<Spawner>>(spawnerDestination.spawnerId())).map<Vector2D>([](const std::shared_ptr<Spawner> spawner) { return spawner->position(); }); },
                            [&spawnersById](const xg::Guid& unconditionalSpawnerDestination) { return (spawnersById | find<std::shared_ptr<Spawner>>(unconditionalSpawnerDestination)).map<Vector2D>([](const std::shared_ptr<Spawner> spawner) { return spawner->position(); }); }
                        );
                    });

                    rallyPosition.foreach([this, &canvas, selectedSpawner](const Vector2D& target) {

                        const auto selectedSpawnerRelativePosition = _cameraRelativeGameManager->absolutePositionToRelative(selectedSpawner->position());
                        const auto targetRelativePosition = _cameraRelativeGameManager->absolutePositionToRelative(target);

                        sf::Vertex line[] =
                        {
                            sf::Vertex(sf::Vector2f(selectedSpawnerRelativePosition.x(), selectedSpawnerRelativePosition.y())),
                            sf::Vertex(sf::Vector2f(targetRelativePosition.x(), targetRelativePosition.y()))
                        };

                        canvas->draw(line, 2, sf::Lines);
                    });
                });
            }

            // Last move units position
            _userControlState->getAbsoluteLastMoveUnitPosition().foreach([&canvas, this](const Vector2D& lastMoveUnitPosition) {
                const double circleRadius(_cameraRelativeGameManager->absoluteLengthToRelative(2.0));
                const Vector2D relativeLastMoveUnitPosition(_cameraRelativeGameManager->absolutePositionToRelative(lastMoveUnitPosition));
                sf::CircleShape cursorDot(circleRadius);
                cursorDot.setPosition(relativeLastMoveUnitPosition.x() - circleRadius, relativeLastMoveUnitPosition.y() - circleRadius);
                cursorDot.setFillColor(sf::Color(255, 255, 255));
                canvas->draw(cursorDot);
            });

            // User selection rectangle
            _userControlState->getAbsoluteSelectionRectangle().foreach([&canvas, this](const Rectangle& userSelection) {
                const auto userSelectionSize(userSelection.size().atModule(_cameraRelativeGameManager->absoluteLengthToRelative(userSelection.size().module())));
                const auto userSelectionLowerRight(_cameraRelativeGameManager->absolutePositionToRelative(userSelection.upperLeftCorner()));
                sf::RectangleShape sfUserSelection(sf::Vector2f(userSelectionSize.x(), userSelectionSize.y()));
                sfUserSelection.setPosition(userSelectionLowerRight.x(), userSelectionLowerRight.y());
                sfUserSelection.setFillColor(sf::Color::Transparent);
                sfUserSelection.setOutlineThickness(2.0);
                canvas->draw(sfUserSelection);
            });
        }

    private:

        void setCircleProperties(sf::CircleShape& circleShape, const double& radius, const Vector2D& position, const sf::Color& innerColor, const double& outsideThickness, const sf::Color& outsideColor)
        {
            float newCircleRadius = (float)radius;
            if (circleShape.getRadius() != newCircleRadius)
            {
                circleShape.setRadius(newCircleRadius);
            }

            sf::Vector2f newCirclePosition(position.x() - radius, position.y() - radius);
            if (circleShape.getPosition() != newCirclePosition)
            {
                circleShape.setPosition(newCirclePosition);
            }

            if (circleShape.getFillColor() != innerColor)
            {
                circleShape.setFillColor(innerColor);
            }

            float newCircleOutlineThickness = (float)outsideThickness;
            if (circleShape.getOutlineThickness() != newCircleOutlineThickness)
            {
                circleShape.setOutlineThickness(newCircleOutlineThickness);
            }

            if (circleShape.getOutlineColor() != outsideColor)
            {
                circleShape.setOutlineColor(outsideColor);
            }
        }

        static const std::vector<sf::Color> _playerColors;

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<UserControlState> _userControlState;
        const std::shared_ptr<CameraRelativeGameManager> _cameraRelativeGameManager;
        xg::Guid _lastDrawnGameStateVersion;
    };
}