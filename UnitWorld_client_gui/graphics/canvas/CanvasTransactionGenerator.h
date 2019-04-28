#pragma once

#include "../canvas/SFMLCanvas.h"

#include <functional>

namespace uw
{
    class CanvasTransactionGenerator
    {
    public:
        CanvasTransactionGenerator(std::shared_ptr<SFMLCanvas> canvas);

        void tryDrawingTransaction(const std::function<void(std::shared_ptr<SFMLDrawingCanvas>)>& drawingFunction);

        ~CanvasTransactionGenerator();

    private:
        std::shared_ptr<SFMLCanvas> _canvas;
    };
}