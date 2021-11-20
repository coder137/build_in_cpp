#pragma once

template <typename T> struct Vector2 {
  Vector2() : x(0), y(0) {}
  Vector2(T xi, T yi) : x(xi), y(yi) {}

  void Print() { std::cout << "X: " << x << " Y: " << y << std::endl; }

  Vector2 operator+(const Vector2 &other) {
    return Vector2(x + other.x, y + other.y);
  };

  T x;
  T y;
};
