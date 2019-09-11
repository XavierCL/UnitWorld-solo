#pragma once

#include "clientgui/networking/ServerCommander.h"

#include "shared/game/GameManager.h"

#include "shared/game/geometry/Vector2D.h"
#include "shared/game/geometry/Rectangle.h"
#include "shared/game/geometry/Circle.h"

#include "commons/Option.hpp"
#include "commons/Guid.hpp"
#include "commons/CollectionPipe.h"

#include <vector>

namespace uw
{
    class UserControlState
    {
    public:
        UserControlState(std::shared_ptr<GameManager> gameManager, std::shared_ptr<ServerCommander> serverCommander) :
            _gameManager(gameManager),
            _serverCommander(serverCommander),
            _leftMouseDownPosition(std::make_shared<Option<const Vector2D>>()),
            _lastMousePosition(std::make_shared<Option<const Vector2D>>()),
            _lastMoveUnitPosition(std::make_shared<Option<const Vector2D>>()),
            _selectedUnits(std::make_shared<std::vector<xg::Guid>>()),
            _lastSelectedSpawnerId(std::make_shared<Option<const xg::Guid>>()),
            _lastSelectedSpawnerAllegence(std::make_shared<Option<SpawnerAllegence>>())
        {}

        void setUserLeftMouseDownPosition(const Vector2D& position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>(position);
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
            _selectedUnits = std::make_shared<std::vector<xg::Guid>>();
        }

        void setUserLeftMouseUpPosition(const Vector2D position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
        }

        void frameHappened()
        {
            _lastMousePosition->foreach([this](const auto& position) {
                _leftMouseDownPosition->foreach([this, position](const Vector2D& leftMouseDownPosition) {
                    const auto selectionRectangle(Rectangle(leftMouseDownPosition, position));

                    _selectedUnits = _gameManager->currentPlayer().map<std::shared_ptr<std::vector<xg::Guid>>>([&selectionRectangle](std::shared_ptr<Player> player) {
                        return player->singuities() | filter<std::shared_ptr<Singuity>>([&selectionRectangle](std::shared_ptr<Singuity> singuity) {
                            return selectionRectangle.contains(singuity->position());
                        }) | map<xg::Guid>([](std::shared_ptr<Singuity> singuity) { return singuity->id(); })
                            | toVector<xg::Guid>();
                    }).getOrElse(std::make_shared<std::vector<xg::Guid>>());
                });
            });

            _lastSelectedSpawnerId->foreach([this](const xg::Guid& spawnerId) {
                (&_gameManager->completeGameState()->spawners() | first<std::shared_ptr<Spawner>>([&spawnerId](std::shared_ptr<Spawner> spawner) {
                    return spawner->id() == spawnerId;
                })).foreach([this](std::shared_ptr<Spawner> foundSpawner) {
                    if (!foundSpawner->hasSameAllegenceState(*_lastSelectedSpawnerAllegence))
                    {
                        _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
                        _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
                    }
                }).orExecute([this]() {
                    _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
                    _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
                });
            });
        }

        void setUserMousePosition(const Vector2D& position)
        {
            _lastMousePosition = std::make_shared<Option<const Vector2D>>(position);
        }

        void setUserRightMouseDownPosition(const Vector2D& position)
        {
            auto completeGameState = _gameManager->completeGameState();
            auto reverseSpawners = &completeGameState->spawners() | reverse<std::shared_ptr<Spawner>>();

            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();

            for (auto spawner : *reverseSpawners)
            {
                Circle spawnerShape(spawner->position(), 20.0);
                auto currentPlayerId = _gameManager->currentPlayer().map<xg::Guid>([](std::shared_ptr<Player> player) { return player->id(); });
                bool spawnerCanBeInterractedWith = currentPlayerId.map<bool>([spawner](const xg::Guid& id) { return spawner->canBeInteractedWithBy(id); }).getOrElse(false);
                if (spawnerShape.contains(position) && spawnerCanBeInterractedWith)
                {
                    _serverCommander->moveUnitsToSpawner(*_selectedUnits, spawner->id());
                    _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>(spawner->id());
                    _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>(spawner->allegence());
                    return;
                }
            }
            _serverCommander->moveUnitsToPosition(*_selectedUnits, position);
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>(position);
        }

        Option<Rectangle> getSelectionRectangle() const
        {
            return _leftMouseDownPosition->flatMap<Rectangle>([this](const Vector2D& leftMouseDown) {
                return _lastMousePosition->map<Rectangle>([&leftMouseDown](const Vector2D& currentMousePosition) {
                    return Rectangle(leftMouseDown, currentMousePosition);
                });
            });
        }

        std::vector<xg::Guid> getSelectedUnits() const
        {
            return *_selectedUnits;
        }

        Option<const Vector2D> getLastMoveUnitPosition() const
        {
            return *_lastMoveUnitPosition;
        }

        Option<const xg::Guid> getLastSelectedSpawnerId() const
        {
            return *_lastSelectedSpawnerId;
        }

    private:
        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<ServerCommander> _serverCommander;

        std::shared_ptr<Option<const Vector2D>> _leftMouseDownPosition;
        std::shared_ptr<Option<const Vector2D>> _lastMousePosition;
        std::shared_ptr<Option<const Vector2D>> _lastMoveUnitPosition;
        std::shared_ptr<Option<const xg::Guid>> _lastSelectedSpawnerId;
        std::shared_ptr<Option<SpawnerAllegence>> _lastSelectedSpawnerAllegence;

        std::shared_ptr<std::vector<xg::Guid>> _selectedUnits;
    };
}