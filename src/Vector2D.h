#ifndef SDL_VECTOR2D_H
#define SDL_VECTOR2D_H

#include <iostream>

class Vector2D
{
public:
    float x;
    float y;

public:
    Vector2D();
    Vector2D(float x, float y);

    Vector2D& Add(const Vector2D& vec);
    Vector2D& Subtract(const Vector2D& vec);
    Vector2D& Multiple(const Vector2D& vec);
    Vector2D& Divide(const Vector2D& vec);

    friend Vector2D& operator+(Vector2D& vec1, Vector2D& vec2);
    friend Vector2D& operator-(Vector2D& vec1, Vector2D& vec2);
    friend Vector2D& operator*(Vector2D& vec1, Vector2D& vec2);
    friend Vector2D& operator/(Vector2D& vec1, Vector2D& vec2);

    Vector2D& operator+=(const Vector2D& vec);
    Vector2D& operator-=(const Vector2D& vec);
    Vector2D& operator*=(const Vector2D& vec);
    Vector2D& operator/=(const Vector2D& vec);

    Vector2D& operator*=(const int& i);
    Vector2D& Zero();

    friend std::ostream& operator<<(std::ostream& stream, const Vector2D& vec);
};

#endif