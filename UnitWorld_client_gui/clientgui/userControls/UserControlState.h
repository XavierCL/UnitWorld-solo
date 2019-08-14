#pragma once

#include "ServerCommander.h"

#include "shared/game/GameManager.h"

#include "shared/game/physics/Vector2D.h"
#include "shared/game/geometry/Rectangle.h"

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
            _selectedUnits(std::make_shared<std::vector<xg::Guid>>())
        {}

        void setUserLeftMouseDownPosition(const Vector2D& position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>(position);
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
            _selectedUnits = std::make_shared<std::vector<xg::Guid>>();
        }

        void setUserLeftMouseUpPosition(const Vector2D position)
        {
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>();
        }

        void setUserMousePosition(const Vector2D& position)
        {
            _lastMousePosition = std::make_shared<Option<const Vector2D>>(position);
            _leftMouseDownPosition->foreach([this, position](const Vector2D& leftMouseDownPosition) {
                const auto selectionRectangle(Rectangle(leftMouseDownPosition, position));

                _selectedUnits = std::make_shared<immer::vector<std::shared_ptr<const Singuity>>>(_gameManager->singuities())
                    | filter<std::shared_ptr<const Singuity>>([this, &selectionRectangle](const std::shared_ptr<const Singuity>& singuity) {
                        return selectionRectangle.contains(singuity->position());
                    }) | map<xg::Guid>([](const std::shared_ptr<const Singuity>& singuity) { return singuity->id(); })
                    | toVector<xg::Guid>();
            });
        }

        void setUserRightMouseDownPosition(const Vector2D& position)
        {
            _serverCommander->moveUnitsToPosition(*_selectedUnits, position);
            _lastMoveUnitPosition = std::make_shared<Option<const Vector2D>>(position);
            _leftMouseDownPosition = std::make_shared<Option<const Vector2D>>();
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
    private:
        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<ServerCommander> _serverCommander;

        std::shared_ptr<Option<const Vector2D>> _leftMouseDownPosition;
        std::shared_ptr<Option<const Vector2D>> _lastMousePosition;
        std::shared_ptr<Option<const Vector2D>> _lastMoveUnitPosition;

        std::shared_ptr<std::vector<xg::Guid>> _selectedUnits;
    };
}