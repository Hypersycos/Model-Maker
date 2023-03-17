#include "vector2.h"
#define _USE_MATH_DEFINES
#include <cmath>
//had to include math.h for M_PI for MSVC2017.
#include <math.h>

Vector2::Vector2(double x, double y)
{
    setSize(x, y);
}

std::pair<double, double> Vector2::getSize()
{
    std::pair<double, double> toReturn;
    toReturn.first = x;
    toReturn.second = y;
    return toReturn;
}

void Vector2::setSize(double x, double y)
{
    this->x = x;
    this->y = y;
}

void Vector2::setSize(std::pair<double, double> size)
{
   setSize(size.first, size.second);
}

Vector2 Vector2::unitVector()
{
    double size = magnitude();
    return Vector2(x / size, y / size);
}

QPointF Vector2::toQPointF()
{
    //-y for QGraphicsScene
    return QPointF(x, -y);
}

Vector2 Vector2::rotate(double rotation)
{
    rotation = 2 * M_PI * rotation / 360;
    Vector2 rotated = Vector2();
    rotated.x = x * std::cos(rotation) - y * std::sin(rotation);
    rotated.y = y * std::cos(rotation) + x * std::sin(rotation);
    return rotated;
}

double Vector2::magnitude()
{
    return sqrt(magnitudeSquared());
}

double Vector2::magnitudeSquared()
{
    return pow(x,2) + pow(y,2);
}

double Vector2::angle() const
{
    if (x > 0 && y > 0)
        return atan(x/y);
    else if (x > 0)
        return atan(-y/x) + M_PI_2;
    else if (y > 0)
        return 2*M_PI - atan(-x/y);
    else
        return atan(x/y) + M_PI;
}

Vector2 Vector2::operator+(const Vector2 operand)
{
    Vector2 toReturn = Vector2();
    toReturn.x = x + operand.x;
    toReturn.y = y + operand.y;
    return toReturn;
}

Vector2 Vector2::operator-(const Vector2 operand)
{
    Vector2 toReturn = Vector2();
    toReturn.x = x - operand.x;
    toReturn.y = y - operand.y;
    return toReturn;
}

Vector2 Vector2::operator*(const Vector2 operand)
{
    Vector2 toReturn = Vector2();
    toReturn.x = x * operand.x;
    toReturn.y = y * operand.y;
    return toReturn;
}

Vector2 Vector2::operator/(const Vector2 operand)
{
    Vector2 toReturn = Vector2();
    toReturn.x = x / operand.x;
    toReturn.y = y / operand.y;
    return toReturn;
}

Vector2 Vector2::operator-(const double operand)
{
    return Vector2(x - operand, y - operand);
}

Vector2 Vector2::operator*(const double operand)
{
    return Vector2(x * operand, y * operand);
}
