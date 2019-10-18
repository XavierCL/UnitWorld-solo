#pragma once

#include "clientShared/networking/ServerCommander.h"

#include "clientgui/cameras/CameraRelativeGameManager.h"

#include "shared/game/geometry/Vector2D.h"
#include "shared/game/geometry/Rectangle.h"
#include "shared/game/geometry/Circle.h"

#include "shared/game/GameManager.h"

#include "commons/Option.hpp"
#include "commons/Guid.hpp"
#include "commons/CollectionPipe.h"

#include <vector>

namespace uw
{
    class UserControlState
    {
    public:
        UserControlState(std::shared_ptr<GameManager> gameManager, std::shared_ptr<CameraRelativeGameManager> cameraRelativeGameManager, std::shared_ptr<ServerCommander> serverCommander) :
            _gameManager(gameManager),
            _cameraRelativeGameManager(cameraRelativeGameManager),
            _serverCommander(serverCommander),
            _leftMouseDownPosition(std::make_shared<Option<const Vector2D>>()),
            _lastMousePosition(std::make_shared<Option<const Vector2D>>()),
            _lastMoveUnitPosition(std::make_shared<Option<const Vector2D>>()),
            _selectedUnits(std::make_shared<std::unordered_set<xg::Guid>>()),
            _selectedSpawners(std::make_shared<std::unordered_set<xg::Guid>>()),
            _lastSelectedSpawnerId(std::make_shared<Option<const xg::Guid>>()),
            _lastSelectedSpawnerAllegence(std::make_shared<Option<SpawnerAllegence>>()),
            _unitGroups(std::vector<std::unordered_set<xg::Guid>>(10)),
            _isLeftShiftKeyPressed(false)
        {}

        void setUserLeftMouseDownPosition(const Vector2D& position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>(_cameraRelativeGameManager->relativePositionToAbsolute(position));
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
            if (!_isLeftShiftKeyPressed)
            {
                _selectedUnits = std::make_shared<std::unordered_set<xg::Guid>>();
            }
        }

        void setUserLeftMouseUpPosition(const Vector2D& position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
            _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>();
        }

        void setUserDoubleClickMouseUpPosition(const Vector2D& position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
            _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>();

            _gameManager->currentPlayer().foreach([this, &position](const std::shared_ptr<Player> currentPlayer) {
                const auto completeGameState = _gameManager->completeGameState();

                const auto currentPlayerSpawners = std::make_shared<std::unordered_set<xg::Guid>>();
                bool clickedOnOwnSpawner = false;

                for (const auto spawner : completeGameState->spawners())
                {
                    if (spawner->isAllegedToPlayer(currentPlayer->id()))
                    {
                        currentPlayerSpawners->emplace(spawner->id());

                        if (_cameraRelativeGameManager->relativeCircleOf(spawner).contains(position))
                        {
                            clickedOnOwnSpawner = true;
                        }
                    }
                }

                if (clickedOnOwnSpawner)
                {
                    _selectedSpawners = currentPlayerSpawners;
                }
            });
        }

        void setUserClickMouseUpPosition(const Vector2D& position, const bool isControlPressed)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();

            _gameManager->currentPlayer().foreach([this, &position, &isControlPressed](const std::shared_ptr<Player> currentPlayer) {
                const auto completeGameState = _gameManager->completeGameState();

                for (const auto spawner : completeGameState->spawners())
                {
                    if (spawner->isAllegedToPlayer(currentPlayer->id()) && _cameraRelativeGameManager->relativeCircleOf(spawner).contains(position))
                    {
                        if (isControlPressed)
                        {
                            if (_selectedSpawners->count(spawner->id()) == 0)
                            {
                                const auto selectedSpawnersWithNewOne = std::make_shared<std::unordered_set<xg::Guid>>(*_selectedSpawners);
                                selectedSpawnersWithNewOne->emplace(spawner->id());
                                _selectedSpawners = selectedSpawnersWithNewOne;
                            }
                            else
                            {
                                const auto selectedSpawnersWithoutLastOne = std::make_shared<std::unordered_set<xg::Guid>>(*_selectedSpawners);
                                selectedSpawnersWithoutLastOne->erase(spawner->id());
                                _selectedSpawners = selectedSpawnersWithoutLastOne;
                            }
                        }
                        else
                        {
                            const auto onlyOneSelectedSpawner = std::make_shared<std::unordered_set<xg::Guid>>();
                            onlyOneSelectedSpawner->emplace(spawner->id());
                            _selectedSpawners = onlyOneSelectedSpawner;
                        }
                        _selectedUnits = std::make_shared<std::unordered_set<xg::Guid>>();
                        return;
                    }
                }
                _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>();
            }).orExecute([this]() { _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>(); });
        }

        void frameHappened()
        {
            const auto currentPlayer = _gameManager->currentPlayer();
            const auto completeGameState = _gameManager->completeGameState();

            _lastMousePosition->foreach([this, currentPlayer](const auto& position) {
                _leftMouseDownPosition->foreach([this, position, currentPlayer](const Vector2D& leftMouseDownPosition) {
                    const Rectangle selectionRectangle(_cameraRelativeGameManager->absolutePositionToRelative(leftMouseDownPosition), _cameraRelativeGameManager->absolutePositionToRelative(position));

                    const auto  currentFrameSelectedUnits = currentPlayer.map<std::shared_ptr<std::unordered_set<xg::Guid>>>([&selectionRectangle, this](std::shared_ptr<Player> player) {
                        return player->singuities() | filter<std::shared_ptr<Singuity>>([&selectionRectangle, this](std::shared_ptr<Singuity> singuity) {
                            const auto singuityRelativeCircle(_cameraRelativeGameManager->relativeCircleOf(singuity));
                            return selectionRectangle.intersectsWith(singuityRelativeCircle);
                        }) | map<xg::Guid>([](std::shared_ptr<Singuity> singuity) { return singuity->id(); })
                        | toUnorderedSet<xg::Guid>();
                    }).getOrElse(std::make_shared<std::unordered_set<xg::Guid>>());
                    if (_isLeftShiftKeyPressed)
                    {
                        currentFrameSelectedUnits->insert(_selectedUnits->begin(), _selectedUnits->end());
                        _selectedUnits = currentFrameSelectedUnits;
                    }
                    else
                    {
                        _selectedUnits = currentFrameSelectedUnits;
                    }
                });
            });

            _lastSelectedSpawnerId->foreach([this, completeGameState, currentPlayer](const xg::Guid& spawnerId) {
                (&completeGameState->spawners() | first<std::shared_ptr<Spawner>>([&spawnerId](std::shared_ptr<Spawner> spawner) {
                    return spawner->id() == spawnerId;
                })).foreach([this, currentPlayer](std::shared_ptr<Spawner> foundSpawner) {
                    currentPlayer.foreach([this, &foundSpawner](const std::shared_ptr<Player>& currentPlayer) {
                        if (!foundSpawner->hasSameAllegenceState(*_lastSelectedSpawnerAllegence, currentPlayer->id()))
                        {
                            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
                            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
                        }
                    });
                }).orExecute([this]() {
                    _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
                    _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
                });
            });

            currentPlayer
                .foreach([this, completeGameState](const std::shared_ptr<Player> currentPlayer) {
                    const auto updatedSelectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>();

                    for (const auto spawner : completeGameState->spawners())
                    {
                        if (spawner->isAllegedToPlayer(currentPlayer->id()) && _selectedSpawners->count(spawner->id()) > 0)
                        {
                            updatedSelectedSpawners->emplace(spawner->id());
                        }
                    }

                    _selectedSpawners = updatedSelectedSpawners;

                }).orExecute([this]() { _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>(); });
        }

        void setUserMousePosition(const Vector2D& position)
        {
            _lastMousePosition = std::make_shared<Option<const Vector2D>>(_cameraRelativeGameManager->relativePositionToAbsolute(position));
        }

        void setUserRightMouseDownPosition(const Vector2D& position)
        {
            auto completeGameState = _gameManager->completeGameState();
            auto currentPlayer = _gameManager->currentPlayer();
            auto reverseSpawners = &completeGameState->spawners() | reverse<std::shared_ptr<Spawner>>();

            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();

            if (!_selectedUnits->empty())
            {
                for (auto spawner : *reverseSpawners)
                {
                    const auto spawnerRelativeCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                    auto currentPlayerId = currentPlayer.map<xg::Guid>([](std::shared_ptr<Player> player) { return player->id(); });
                    bool spawnerCanBeInterractedWith = currentPlayerId.map<bool>([spawner](const xg::Guid& id) { return spawner->canBeInteractedWithBy(id); }).getOrElse(false);
                    if (spawnerRelativeCircle.contains(position) && spawnerCanBeInterractedWith)
                    {
                        _serverCommander->moveUnitsToSpawner(*_selectedUnits, spawner->id());
                        _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>(spawner->id());
                        _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>(spawner->allegence());
                        return;
                    }
                }
                const Vector2D absoluteMousePosition(_cameraRelativeGameManager->relativePositionToAbsolute(position));
                _serverCommander->moveUnitsToPosition(*_selectedUnits, absoluteMousePosition);
                _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>(absoluteMousePosition);
            }
            else
            {
                for (auto spawner : *reverseSpawners)
                {
                    const auto spawnerRelativeCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                    auto currentPlayerId = currentPlayer.map<xg::Guid>([](std::shared_ptr<Player> player) { return player->id(); });
                    if (spawnerRelativeCircle.contains(position))
                    {
                        _serverCommander->setSpawnersRally(*_selectedSpawners, MobileUnitDestination(spawner->id()));
                        return;
                    }
                }
                const Vector2D absoluteMousePosition(_cameraRelativeGameManager->relativePositionToAbsolute(position));
                _serverCommander->setSpawnersRally(*_selectedSpawners, MobileUnitDestination(absoluteMousePosition));
            }
        }

        Option<Rectangle> getAbsoluteSelectionRectangle() const
        {
            return _leftMouseDownPosition->flatMap<Rectangle>([this](const Vector2D& leftMouseDown) {
                return _lastMousePosition->map<Rectangle>([&leftMouseDown](const Vector2D& currentMousePosition) {
                    return Rectangle(leftMouseDown, currentMousePosition);
                });
            });
        }

        std::unordered_set<xg::Guid> getSelectedUnits() const
        {
            return *_selectedUnits;
        }

        std::unordered_set<xg::Guid> getSelectedSpawners() const
        {
            return *_selectedSpawners;
        }

        Option<const Vector2D> getAbsoluteLastMoveUnitPosition() const
        {
            return *_lastMoveUnitPosition;
        }

        Option<const xg::Guid> getLastSelectedSpawnerId() const
        {
            return *_lastSelectedSpawnerId;
        }

		void setSelectedUnitsToUnitGroup(const int& unitGroupNumber)
		{
            const auto sharedSelectedUnitsCopy = _selectedUnits;
            _unitGroups[unitGroupNumber] = *sharedSelectedUnitsCopy;
		}

		void setUnitGroupToSelectedUnits(const int& unitGroupNumber)
		{
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
            _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>();

            if (_isLeftShiftKeyPressed)
            {
                const auto sharedSelectedUnits = std::make_shared<std::unordered_set<xg::Guid>>(_unitGroups[unitGroupNumber]);
                sharedSelectedUnits->insert(_selectedUnits->begin(), _selectedUnits->end());
                _selectedUnits = sharedSelectedUnits;
            }
            else
            {
                _selectedUnits = std::make_shared<std::unordered_set<xg::Guid>>(_unitGroups[unitGroupNumber]);
            }

        }

        void userPressedLeftShift()
        {
            _isLeftShiftKeyPressed = true;
        }

        void userReleasedLeftShift()
        {
            _isLeftShiftKeyPressed = false;
        }

        void selectAllUnits()
        {
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
            _selectedSpawners = std::make_shared<std::unordered_set<xg::Guid>>();

            _selectedUnits = _gameManager->currentPlayer().map<std::shared_ptr<std::unordered_set<xg::Guid>>>([](std::shared_ptr<Player> player) {
                return player->singuities()
                    | map<xg::Guid>([](std::shared_ptr<Singuity> singuity) { return singuity->id(); })
                    | toUnorderedSet<xg::Guid>();
            }).getOrElse(std::make_shared<std::unordered_set<xg::Guid>>());
        }

    private:
        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<CameraRelativeGameManager> _cameraRelativeGameManager;
        const std::shared_ptr<ServerCommander> _serverCommander;

        std::shared_ptr<Option<const Vector2D>> _leftMouseDownPosition;
        std::shared_ptr<Option<const Vector2D>> _lastMousePosition;
        std::shared_ptr<Option<const Vector2D>> _lastMoveUnitPosition;
        std::shared_ptr<Option<const xg::Guid>> _lastSelectedSpawnerId;
        std::shared_ptr<Option<SpawnerAllegence>> _lastSelectedSpawnerAllegence;

        std::shared_ptr<std::unordered_set<xg::Guid>> _selectedUnits;
        std::shared_ptr<std::unordered_set<xg::Guid>> _selectedSpawners;
        std::vector<std::unordered_set<xg::Guid>> _unitGroups;

        bool _isLeftShiftKeyPressed;
    };
}