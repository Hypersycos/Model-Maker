#ifndef VECTOR2_H
#define VECTOR2_H

#include <tuple>
#include <QPointF>

class Vector2
{
public:
    Vector2(double x = 0, double y = 0);
    std::pair<double, double> getSize();
    void setSize(std::pair<double, double> size);
    void setSize(double x, double y);
    Vector2 unitVector();
    double magnitude();
    double magnitudeSquared();
    double angle() const;
    Vector2 rotate(double rotation);
    double x;
    double y;

    QPointF toQPointF();
    Vector2 operator+ (const Vector2 operand);
    Vector2 operator- (const Vector2 operand);
    Vector2 operator* (const Vector2 operand);
    Vector2 operator/ (const Vector2 operand);

    Vector2 operator- (const double operand);
    Vector2 operator* (const double operand);
};

#endif // VECTOR2_H
