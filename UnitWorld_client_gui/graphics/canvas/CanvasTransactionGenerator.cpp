#include "CanvasTransactionGenerator.h"

using namespace uw;

CanvasTransactionGenerator::CanvasTransactionGenerator(std::shared_ptr<SFMLCanvas> canvas) :
    _canvas(canvas)
{}

void CanvasTransactionGenerator::tryDrawingTransaction(const std::function<void(std::shared_ptr<SFMLDrawingCanvas>)>& drawingFunction)
{
    drawingFunction(_canvas);
    _canvas->display();
}

CanvasTransactionGenerator::~CanvasTransactionGenerator()
{
    _canvas->close();
}