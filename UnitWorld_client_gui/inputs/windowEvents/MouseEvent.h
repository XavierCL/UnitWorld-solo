#pragma once

#include "../WindowContext.h"

class MouseEvent
{
public:
    virtual void execute(const WindowContext &context) = 0;


};