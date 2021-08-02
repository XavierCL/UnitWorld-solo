#pragma once

#include "WindowStats.h"

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
        WindowManager(const unsigned int& graphicsFramePerSecond, std::shared_ptr<GameDrawer> gameDrawer, std::shared_ptr<CanvasTransactionGenerator> canvasTransactionGenerator, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<WindowInputs> windowInputs, std::shared_ptr<WindowStats> windowStats) :
            _msPerFrame(1000 / graphicsFramePerSecond),
            _gameDrawer(gameDrawer),
            _canvasTransactionGenerator(canvasTransactionGenerator),
            _window(window),
            _windowInputs(windowInputs),
            _windowStats(windowStats)
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
                    _windowStats->feedFrameDuration(frameTimeInMs);

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
                else if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    Vector2D mousePosition(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
                    double delta(event.mouseWheelScroll.delta);
                    _windowInputs->setUserMouseWheelScroll(delta, mousePosition);
                }
                else if(event.type == sf::Event::KeyPressed)
                {
                    if(event.key.code == sf::Keyboard::LControl)
                    {
                        _windowInputs->userPressedLeftControl();
                    }
                    else if(event.key.code == sf::Keyboard::LShift)
                    {
                        _windowInputs->userPressedLeftShift();
                    }
                    else if(event.key.code == sf::Keyboard::Escape)
                    {
                        _window->close();
                    }
                    else if (event.key.code == sf::Keyboard::A)
                    {
                        _windowInputs->userPressedAKey();
                    }
                    else if(event.key.code == sf::Keyboard::Num0)
                    {
                        _windowInputs->userPressedNumber(0);
                    }
                    else if(event.key.code == sf::Keyboard::Num1)
                    {
                        _windowInputs->userPressedNumber(1);
                    }
                    else if(event.key.code == sf::Keyboard::Num2)
                    {
                        _windowInputs->userPressedNumber(2);
                    }
                    else if(event.key.code == sf::Keyboard::Num3)
                    {
                        _windowInputs->userPressedNumber(3);
                    }
                    else if(event.key.code == sf::Keyboard::Num4)
                    {
                        _windowInputs->userPressedNumber(4);
                    }
                    else if(event.key.code == sf::Keyboard::Num5)
                    {
                        _windowInputs->userPressedNumber(5);
                    }
                    else if(event.key.code == sf::Keyboard::Num6)
                    {
                        _windowInputs->userPressedNumber(6);
                    }
                    else if(event.key.code == sf::Keyboard::Num7)
                    {
                        _windowInputs->userPressedNumber(7);
                    }
                    else if(event.key.code == sf::Keyboard::Num8)
                    {
                        _windowInputs->userPressedNumber(8);
                    }
                    else if(event.key.code == sf::Keyboard::Num9)
                    {
                        _windowInputs->userPressedNumber(9);
                    }
                    else if (event.key.code == sf::Keyboard::Add)
                    {
                        _windowInputs->userPressedAddKey();
                    }
                    else if (event.key.code == sf::Keyboard::Subtract)
                    {
                        _windowInputs->userPressedSubstractKey();
                    }
                }
                else if(event.type == sf::Event::KeyReleased)
                {
                    if(event.key.code == sf::Keyboard::LControl)
                    {
                        _windowInputs->userReleasedLeftControl();
                    }
                    else if(event.key.code == sf::Keyboard::LShift)
                    {
                        _windowInputs->userReleasedLeftShift();
                    }
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
        const std::shared_ptr<WindowStats> _windowStats;
    };
}