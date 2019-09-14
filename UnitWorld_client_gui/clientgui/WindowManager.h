#pragma once

#include "userControls/WindowInputs.h"

#include "graphics/GameDrawer.h"
#include "graphics/canvas/CanvasTransactionGenerator.h"

#include "shared/game/GameManager.h"

#include <SFML/Graphics.hpp>

#include <chrono>

namespace uw
{
    class WindowManager
    {
    public:
        WindowManager(const unsigned int& graphicsFramePerSecond, std::shared_ptr<GameDrawer> gameDrawer, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<WindowInputs> windowInputs) :
            _msPerFrame(1000 / graphicsFramePerSecond),
            _gameDrawer(gameDrawer),
            _canvasTransactionGenerator(canvasTransactionGenerator),
            _window(window),
            _windowInputs(windowInputs)
        {}

        void startSync()
        {
            _window->setActive(false);

            while (_window->isOpen())
            {
                const auto startFrameTime = std::chrono::steady_clock::now();

                processEvents();

                if (_window->isOpen())
                {
                    loopGraphics();

                    const auto endFrameTime = std::chrono::steady_clock::now();

                    const auto frameTimeInMs = (unsigned int)std::chrono::duration<double, std::milli>(endFrameTime - startFrameTime).count();

                    if (frameTimeInMs < _msPerFrame)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(_msPerFrame - frameTimeInMs));
                    }
                }
            }
        }

    private:

        void processEvents()
        {
            sf::Event event;
            while (_window->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    _window->close();
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        Vector2D mousePosition(event.mouseButton.x, event.mouseButton.y);
                        _windowInputs->setUserLeftMouseDownPosition(mousePosition);
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        Vector2D mousePosition(event.mouseButton.x, event.mouseButton.y);
                        _windowInputs->setUserRightMouseDownPosition(mousePosition);
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        Vector2D mousePosition(event.mouseButton.x, event.mouseButton.y);
                        _windowInputs->setUserLeftMouseUpPosition(mousePosition);
                    }
                }
                else if (event.type == sf::Event::MouseMoved)
                {
                    Vector2D mousePosition(event.mouseMove.x, event.mouseMove.y);
                    _windowInputs->setUserMousePosition(mousePosition);
                }
                else if (event.type == sf::Event::MouseWheelScrollEvent)
                {
                    Vector2D mousePosition(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                    double delta(event.mouseWheelScroll.delta);
                    _windowInputs->setUserMouseWheelScroll(delta, mousePosition);
                }
            }
            _windowInputs->frameHappened();
        }

        void loopGraphics()
        {
            _canvasTransactionGenerator->tryDrawingTransaction([this](std::shared_ptr<SFMLDrawingCanvas> canvas)
            {
                _gameDrawer->draw(canvas);
            });
        }

        const unsigned int _msPerFrame;
        const std::shared_ptr<GameDrawer> _gameDrawer;
        const std::shared_ptr<CanvasTransactionGenerator> _canvasTransactionGenerator;
        const std::shared_ptr<sf::RenderWindow> _window;
        const std::shared_ptr<WindowInputs> _windowInputs;
    };
}