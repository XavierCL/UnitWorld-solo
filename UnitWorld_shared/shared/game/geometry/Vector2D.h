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
        Vector2D operator+(const Vector2D& other) const;
        Vector2D operator-(const Vector2D& other) const;
        bool operator==(const Vector2D& other) const;
        const double module() const;
        const double moduleSq() const;
        Vector2D maxAt(const double& maxModule) const;
        Vector2D atModule(const double& module) const;

        Vector2D divide(const double& divided, const double& max) const;
    private:

        double _x;
        double _y;
    };
}