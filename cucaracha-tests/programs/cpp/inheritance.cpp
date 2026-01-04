// Test class inheritance and method overriding
// No stdlib dependencies

class Shape {
protected:
    int x, y;

public:
    Shape() : x(0), y(0) {}
    Shape(int x, int y) : x(x), y(y) {}
    
    int getX() const { return x; }
    int getY() const { return y; }
    void move(int dx, int dy) { x += dx; y += dy; }
    
    // Base implementation of area
    int area() const { return 0; }
};

class Rectangle : public Shape {
protected:
    int width, height;

public:
    Rectangle() : Shape(), width(0), height(0) {}
    Rectangle(int x, int y, int w, int h) : Shape(x, y), width(w), height(h) {}
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    int area() const { return width * height; }
    int perimeter() const { return 2 * (width + height); }
};

class Square : public Rectangle {
public:
    Square() : Rectangle() {}
    Square(int x, int y, int side) : Rectangle(x, y, side, side) {}
    
    int getSide() const { return width; }
    void setSide(int s) { width = s; height = s; }
};

int main() {
    Shape s(5, 10);
    if (s.getX() != 5) return 1;
    if (s.getY() != 10) return 2;
    if (s.area() != 0) return 3;
    
    Rectangle r(0, 0, 4, 5);
    if (r.area() != 20) return 4;
    if (r.perimeter() != 18) return 5;
    
    r.move(3, 4);
    if (r.getX() != 3) return 6;
    if (r.getY() != 4) return 7;
    
    Square sq(1, 1, 7);
    if (sq.area() != 49) return 8;
    if (sq.getSide() != 7) return 9;
    
    sq.setSide(5);
    if (sq.area() != 25) return 10;
    
    // Verify inherited methods work
    if (sq.getWidth() != 5) return 11;
    if (sq.getHeight() != 5) return 12;
    
    return 0; // success
}
