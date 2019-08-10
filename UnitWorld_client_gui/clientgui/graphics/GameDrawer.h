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
        GameDrawer(std::shared_ptr<GameManager> gameManager, std::shared_ptr<UserControlState> userControlState):
            _gameManager(gameManager),
            _userControlState(userControlState)
        {}

        void draw(std::shared_ptr<SFMLDrawingCanvas> canvas)
        {
            const auto localSinguities = std::make_shared<immer::vector<std::shared_ptr<const Singuity>>>(_gameManager->singuities())
                | toUnorderedMap<xg::Guid, std::shared_ptr<const Singuity>>([](const std::shared_ptr<const Singuity>& singuity) {
                    return singuity->id();
                });

            // Last move units position
            _userControlState->getLastMoveUnitPosition().foreach([&canvas](const Vector2D& lastMoveUnitPosition) {
                sf::CircleShape cursorDot(2);
                cursorDot.setPosition(round(lastMoveUnitPosition.x()), round(lastMoveUnitPosition.y()));
                cursorDot.setFillColor(sf::Color(255, 255, 255));
                canvas->draw(cursorDot);
            });

            // Selected unit white aura
            for (const auto selectedUnitId : _userControlState->getSelectedUnits())
            {
                (localSinguities | find<std::shared_ptr<const Singuity>>(selectedUnitId)).foreach([&canvas](const std::shared_ptr<const Singuity>& singuity) {
                    sf::CircleShape selectedUnitAura(5);
                    selectedUnitAura.setPosition(round(singuity->position().x()), round(singuity->position().y()));
                    selectedUnitAura.setFillColor(sf::Color(255, 255, 255));
                    canvas->draw(selectedUnitAura);
                });
            }

            // Units
            for (auto singuity : *(localSinguities | mapValues<std::shared_ptr<const Singuity>>()))
            {
                canvas->draw(*(GraphicalSinguity(*singuity).drawable()));
            }

            // User selection rectangle
            _userControlState->getSelectionRectangle().foreach([&canvas](const Rectangle& userSelection) {
                const auto userSelectionSize(userSelection.size());
                const auto userSelectionCenter(userSelection.center());
                sf::RectangleShape sfUserSelection(sf::Vector2f(userSelectionSize.x(), userSelectionSize.y()));
                sfUserSelection.setPosition(userSelectionCenter.x(), userSelectionCenter.y());
                sfUserSelection.setFillColor(sf::Color::Transparent);
                sfUserSelection.setOutlineThickness(1);
                canvas->draw(sfUserSelection);
            });
        }

    private:

        const std::shared_ptr<GameManager> _gameManager;
        const std::shared_ptr<UserControlState> _userControlState;
    };
}