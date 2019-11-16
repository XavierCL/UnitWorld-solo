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
        }

        void frameHappened()
        {
            _lastMousePosition->foreach([this](const auto& position) {
                _leftMouseDownPosition->foreach([this, position](const Vector2D& leftMouseDownPosition) {
                    const Rectangle selectionRectangle(_cameraRelativeGameManager->absolutePositionToRelative(leftMouseDownPosition), _cameraRelativeGameManager->absolutePositionToRelative(position));

                    const auto  currentFrameSelectedUnits = _gameManager->currentPlayer().map<std::shared_ptr<std::unordered_set<xg::Guid>>>([&selectionRectangle, this](std::shared_ptr<Player> player) {
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

            _lastSelectedSpawnerId->foreach([this](const xg::Guid& spawnerId) {
                (&_gameManager->completeGameState()->spawners() | first<std::shared_ptr<Spawner>>([&spawnerId](std::shared_ptr<Spawner> spawner) {
                    return spawner->id() == spawnerId;
                })).foreach([this](std::shared_ptr<Spawner> foundSpawner) {
                    _gameManager->currentPlayer().foreach([this, &foundSpawner](const std::shared_ptr<Player>& currentPlayer) {
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
        }

        void setUserMousePosition(const Vector2D& position)
        {
            _lastMousePosition = std::make_shared<Option<const Vector2D>>(_cameraRelativeGameManager->relativePositionToAbsolute(position));
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
                const auto spawnerRelativeCircle(_cameraRelativeGameManager->relativeCircleOf(spawner));
                auto currentPlayerId = _gameManager->currentPlayer().map<xg::Guid>([](std::shared_ptr<Player> player) { return player->id(); });
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

        Option<const Vector2D> getAbsoluteLastMoveUnitPosition() const
        {
            return *_lastMoveUnitPosition;
        }

        Option<const xg::Guid> getLastSelectedSpawnerId() const
        {
            return *_lastSelectedSpawnerId;
        }

        void addSelectedUnitsToUnitGroup(const int& unitGroupNumber)
        {
            _unitGroups[unitGroupNumber] = *_selectedUnits;
        }

        void setSelectedUnitToUnitGroup(const int& unitGroupNumber)
        {
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
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _lastSelectedSpawnerId = std::make_shared<Option<const xg::Guid>>();
            _lastSelectedSpawnerAllegence = std::make_shared<Option<SpawnerAllegence>>();
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
        std::vector<std::unordered_set<xg::Guid>> _unitGroups;

        bool _isLeftShiftKeyPressed;
    };
}