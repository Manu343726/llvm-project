// Test operator overloading
// No stdlib dependencies

class Vector2D {
private:
    int x, y;

public:
    Vector2D() : x(0), y(0) {}
    Vector2D(int x, int y) : x(x), y(y) {}
    
    int getX() const { return x; }
    int getY() const { return y; }
    
    // Arithmetic operators
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }
    
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    
    Vector2D operator*(int scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }
    
    // Compound assignment operators
    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vector2D& operator*=(int scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    // Comparison operators
    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }
    
    // Unary operators
    Vector2D operator-() const {
        return Vector2D(-x, -y);
    }
    
    // Prefix increment/decrement
    Vector2D& operator++() {
        ++x; ++y;
        return *this;
    }
    
    Vector2D& operator--() {
        --x; --y;
        return *this;
    }
    
    // Subscript operator
    int operator[](int index) const {
        if (index == 0) return x;
        return y;
    }
    
    // Dot product as function call operator
    int operator()(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
};

int main() {
    Vector2D v1(3, 4);
    Vector2D v2(1, 2);
    
    // Test arithmetic operators
    Vector2D v3 = v1 + v2;
    if (v3.getX() != 4 || v3.getY() != 6) return 1;
    
    Vector2D v4 = v1 - v2;
    if (v4.getX() != 2 || v4.getY() != 2) return 2;
    
    Vector2D v5 = v1 * 2;
    if (v5.getX() != 6 || v5.getY() != 8) return 3;
    
    // Test compound assignment
    Vector2D v6(10, 20);
    v6 += v1;
    if (v6.getX() != 13 || v6.getY() != 24) return 4;
    
    v6 -= v2;
    if (v6.getX() != 12 || v6.getY() != 22) return 5;
    
    v6 *= 2;
    if (v6.getX() != 24 || v6.getY() != 44) return 6;
    
    // Test comparison operators
    Vector2D v7(3, 4);
    if (!(v1 == v7)) return 7;
    if (v1 != v7) return 8;
    if (v1 == v2) return 9;
    if (!(v1 != v2)) return 10;
    
    // Test unary operator
    Vector2D v8 = -v1;
    if (v8.getX() != -3 || v8.getY() != -4) return 11;
    
    // Test prefix increment
    Vector2D v9(5, 5);
    ++v9;
    if (v9.getX() != 6 || v9.getY() != 6) return 12;
    
    --v9;
    if (v9.getX() != 5 || v9.getY() != 5) return 13;
    
    // Test subscript operator
    if (v1[0] != 3) return 14;
    if (v1[1] != 4) return 15;
    
    // Test function call operator (dot product)
    int dot = v1(v2);  // 3*1 + 4*2 = 11
    if (dot != 11) return 16;
    
    return 0; // success
}
