#include "Spawner.h"

using namespace uw;

std::vector<Vector2D> Spawner::UNIT_SPAWN_DIRECTION {
    Vector2D(sqrt(3.0) / 2.0, -1.0 / 2.0),
    Vector2D(-1.0, 0.0),
    Vector2D(sqrt(3.0) / 2.0, 1.0 / 2.0),
    Vector2D(-1.0 / 2.0, -sqrt(3.0) / 2.0),
    Vector2D(0.0, 1.0),
    Vector2D(1.0 / 2.0, -sqrt(3.0) / 2.0),
    Vector2D(-sqrt(3.0) / 2.0, 1.0 / 2.0),
    Vector2D(1.0, 0.0),
    Vector2D(-sqrt(3.0) / 2.0, -1.0 / 2.0),
    Vector2D(1.0 / 2.0, sqrt(3.0) / 2.0),
    Vector2D(0.0, -1.0),
    Vector2D(-1.0 / 2.0, sqrt(3.0) / 2.0)
};