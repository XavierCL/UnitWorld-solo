#pragma once

#include "../canvas/SFMLCanvas.h"

#include <atomic>
#include <functional>

namespace uw
{
    class CanvasTransactionGenerator
    {
    public:
        CanvasTransactionGenerator(std::shared_ptr<SFMLCanvas> canvas) :
            _canvas(canvas)
        {}

        void tryDrawingTransaction(const std::function<void(std::shared_ptr<SFMLDrawingCanvas>)>& drawingFunction)
        {
            drawingFunction(_canvas);
            _canvas->display();
        }

        ~CanvasTransactionGenerator()
        {
            _canvas->close();
        }

    private:
        std::shared_ptr<SFMLCanvas> _canvas;
    };
}