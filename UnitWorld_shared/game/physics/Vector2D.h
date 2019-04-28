#pragma once

namespace uw
{
    class Vector2D
    {
    public:
        Vector2D(const double& x = 0, const double& y = 0);

        double& x();
        const double x() const;
        void x(const double& newX);
        double& y();
        const double y() const;
        void y(const double& newY);

        const double distance(const Vector2D& other) const;
        const double distanceSq(const Vector2D& other) const;
        void operator+=(const Vector2D& other);
        const double module() const;
        const double moduleSq() const;
        Vector2D& maxAt(const double& maxModule);
    private:

        double _x;
        double _y;
    };
}