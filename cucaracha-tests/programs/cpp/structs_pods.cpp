// Test struct vs class, POD types, and aggregate initialization
// No stdlib dependencies

// Simple struct (public by default)
struct Point {
    int x;
    int y;
};

// Struct with methods
struct Vector2D {
    int x;
    int y;
    
    int dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
    
    int lengthSquared() const {
        return x * x + y * y;
    }
};

// Struct with constructor
struct Color {
    int r, g, b;
    
    Color() : r(0), g(0), b(0) {}
    Color(int r, int g, int b) : r(r), g(g), b(b) {}
    
    int brightness() const {
        return (r + g + b) / 3;
    }
};

// Nested struct
struct Rectangle {
    Point topLeft;
    Point bottomRight;
    
    int width() const {
        return bottomRight.x - topLeft.x;
    }
    
    int height() const {
        return bottomRight.y - topLeft.y;
    }
    
    int area() const {
        return width() * height();
    }
};

// Class (private by default)
class PrivatePoint {
    int x;
    int y;

public:
    PrivatePoint() : x(0), y(0) {}
    PrivatePoint(int x, int y) : x(x), y(y) {}
    
    int getX() const { return x; }
    int getY() const { return y; }
};

// Union test
union IntOrFloat {
    int i;
    float f;
};

// Bit fields
struct Flags {
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    unsigned int flag3 : 1;
    unsigned int value : 5;  // 5 bits for value (0-31)
};

// Anonymous struct members
struct Container {
    int id;
    struct {
        int width;
        int height;
    };  // Anonymous struct - members accessible directly
};

int main() {
    // Test simple struct
    Point p1;
    p1.x = 10;
    p1.y = 20;
    if (p1.x != 10) return 1;
    if (p1.y != 20) return 2;
    
    // Aggregate initialization
    Point p2 = {30, 40};
    if (p2.x != 30) return 3;
    if (p2.y != 40) return 4;
    
    // C++11 uniform initialization
    Point p3{50, 60};
    if (p3.x != 50) return 5;
    
    // Test struct with methods
    Vector2D v1{3, 4};
    Vector2D v2{1, 2};
    if (v1.dot(v2) != 11) return 6;  // 3*1 + 4*2 = 11
    if (v1.lengthSquared() != 25) return 7;  // 3*3 + 4*4 = 25
    
    // Test struct with constructor
    Color black;
    if (black.r != 0 || black.g != 0 || black.b != 0) return 8;
    
    Color red(255, 0, 0);
    if (red.r != 255) return 9;
    if (red.brightness() != 85) return 10;  // 255/3 = 85
    
    Color white(255, 255, 255);
    if (white.brightness() != 255) return 11;
    
    // Test nested struct
    Rectangle rect;
    rect.topLeft.x = 0;
    rect.topLeft.y = 0;
    rect.bottomRight.x = 10;
    rect.bottomRight.y = 5;
    
    if (rect.width() != 10) return 12;
    if (rect.height() != 5) return 13;
    if (rect.area() != 50) return 14;
    
    // Nested aggregate initialization
    Rectangle rect2 = {{0, 0}, {20, 10}};
    if (rect2.area() != 200) return 15;
    
    // Test class vs struct (access control)
    PrivatePoint pp(100, 200);
    if (pp.getX() != 100) return 16;
    if (pp.getY() != 200) return 17;
    
    // Test union
    IntOrFloat u;
    u.i = 42;
    if (u.i != 42) return 18;
    
    // Test bit fields
    Flags flags;
    flags.flag1 = 1;
    flags.flag2 = 0;
    flags.flag3 = 1;
    flags.value = 15;
    
    if (flags.flag1 != 1) return 19;
    if (flags.flag2 != 0) return 20;
    if (flags.flag3 != 1) return 21;
    if (flags.value != 15) return 22;
    
    // Test bit field overflow (value is 5 bits, max 31)
    flags.value = 31;
    if (flags.value != 31) return 23;
    
    // Test anonymous struct
    Container c;
    c.id = 1;
    c.width = 100;   // Direct access to anonymous struct member
    c.height = 50;
    
    if (c.id != 1) return 24;
    if (c.width != 100) return 25;
    if (c.height != 50) return 26;
    
    // Test array of structs
    Point points[3] = {{1, 2}, {3, 4}, {5, 6}};
    int sumX = 0;
    for (int i = 0; i < 3; i++) {
        sumX += points[i].x;
    }
    if (sumX != 9) return 27;  // 1 + 3 + 5 = 9
    
    return 0; // success
}
