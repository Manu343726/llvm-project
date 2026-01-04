// Test namespaces and scope resolution
// No stdlib dependencies

namespace Math {
    int add(int a, int b) {
        return a + b;
    }
    
    int subtract(int a, int b) {
        return a - b;
    }
    
    namespace Constants {
        const int PI_APPROX = 3;
        const int E_APPROX = 3;
        const int GOLDEN_RATIO_APPROX = 2;
    }
    
    class Calculator {
    private:
        int result;
    public:
        Calculator() : result(0) {}
        void add(int n) { result += n; }
        void subtract(int n) { result -= n; }
        int getResult() const { return result; }
    };
}

namespace Geometry {
    int rectangleArea(int w, int h) {
        return w * h;
    }
    
    int squareArea(int side) {
        return side * side;
    }
    
    class Point {
    public:
        int x, y;
        Point() : x(0), y(0) {}
        Point(int x, int y) : x(x), y(y) {}
    };
}

// Nested namespace usage
namespace App {
    namespace Utils {
        int clamp(int value, int minVal, int maxVal) {
            if (value < minVal) return minVal;
            if (value > maxVal) return maxVal;
            return value;
        }
    }
}

// Anonymous namespace (internal linkage)
namespace {
    int internalCounter = 0;
    
    void incrementInternal() {
        internalCounter++;
    }
}

int main() {
    // Test namespace functions
    if (Math::add(3, 4) != 7) return 1;
    if (Math::subtract(10, 4) != 6) return 2;
    
    // Test nested namespace constants
    if (Math::Constants::PI_APPROX != 3) return 3;
    
    // Test namespace class
    Math::Calculator calc;
    calc.add(10);
    calc.subtract(3);
    if (calc.getResult() != 7) return 4;
    
    // Test Geometry namespace
    if (Geometry::rectangleArea(4, 5) != 20) return 5;
    if (Geometry::squareArea(6) != 36) return 6;
    
    Geometry::Point p1(3, 4);
    if (p1.x != 3 || p1.y != 4) return 7;
    
    // Test deeply nested namespace
    if (App::Utils::clamp(5, 0, 10) != 5) return 8;
    if (App::Utils::clamp(-5, 0, 10) != 0) return 9;
    if (App::Utils::clamp(15, 0, 10) != 10) return 10;
    
    // Test anonymous namespace
    if (internalCounter != 0) return 11;
    incrementInternal();
    if (internalCounter != 1) return 12;
    incrementInternal();
    incrementInternal();
    if (internalCounter != 3) return 13;
    
    // Using declaration test
    using Math::add;
    if (add(100, 200) != 300) return 14;
    
    // Using namespace in block scope
    {
        using namespace Geometry;
        Point p2(10, 20);
        if (p2.x != 10) return 15;
    }
    
    return 0; // success
}
