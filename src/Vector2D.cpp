#include "Vector2D.h"

Vector2D::Vector2D()
{
    x = 0.0f;
    y = 0.0f;
}

Vector2D::Vector2D(float x, float y)
{
    this->x = x;
    this->y = y;
}

Vector2D &Vector2D::Add(const Vector2D &vec)
{
    this->x += vec.x;
    this->y += vec.y;
    return *this;
}
Vector2D &Vector2D::Subtract(const Vector2D &vec)
{
    this->x -= vec.x;
    this->y -= vec.y;
    return *this;
}
Vector2D &Vector2D::Multiple(const Vector2D &vec)
{
    this->x *= vec.x;
    this->y *= vec.y;
    return *this;
}
Vector2D &Vector2D::Divide(const Vector2D &vec)
{
    this->x /= vec.x;
    this->y /= vec.y;
    return *this;
}

Vector2D &operator+(Vector2D &vec1, Vector2D &vec2)
{
    return vec1.Add(vec2);
}
Vector2D &operator-(Vector2D &vec1, Vector2D &vec2)
{
    return vec1.Subtract(vec2);
}
Vector2D &operator*(Vector2D &vec1, Vector2D &vec2)
{
    return vec1.Multiple(vec2);
}
Vector2D &operator/(Vector2D &vec1, Vector2D &vec2)
{
    return vec1.Divide(vec2);
}

Vector2D &Vector2D::operator+=(const Vector2D &vec)
{
    return this->Add(vec);
}
Vector2D &Vector2D::operator-=(const Vector2D &vec)
{
    return this->Subtract(vec);
}
Vector2D &Vector2D::operator*=(const Vector2D &vec)
{
    return this->Multiple(vec);
}
Vector2D &Vector2D::operator/=(const Vector2D &vec)
{
    return this->Divide(vec);
}

Vector2D &Vector2D::operator*=(const int &i)
{
    this->x *= i;
    this->y *= i;
    return *this;
}
Vector2D &Vector2D::Zero()
{
    this->x = 0.0f;
    this->y = 0.0f;
    return *this;
}

std::ostream &operator<<(std::ostream &stream, const Vector2D &vec)
{
    stream << "(" << vec.x << "," << vec.y << ")";
    return stream;
}