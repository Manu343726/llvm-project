// Test default arguments in functions and methods
// No stdlib dependencies

// Function with default arguments
int add(int a, int b = 10, int c = 20) {
    return a + b + c;
}

// Multiple function overloads (not default args - overloading test)
int multiply(int a) {
    return a * 2;
}

int multiply(int a, int b) {
    return a * b;
}

int multiply(int a, int b, int c) {
    return a * b * c;
}

// Class with default arguments in constructor and methods
class Rectangle {
private:
    int width;
    int height;

public:
    // Constructor with default arguments
    Rectangle(int w = 1, int h = 1) : width(w), height(h) {}
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int area() const { return width * height; }
    
    // Method with default argument
    void scale(int factor = 2) {
        width *= factor;
        height *= factor;
    }
    
    // Method with multiple default arguments
    void resize(int newWidth = 10, int newHeight = 10) {
        width = newWidth;
        height = newHeight;
    }
};

// Class demonstrating method overloading
class Calculator {
public:
    int compute(int a) {
        return a;
    }
    
    int compute(int a, int b) {
        return a + b;
    }
    
    int compute(int a, int b, int c) {
        return a + b + c;
    }
    
    // Overload with different parameter types
    long compute(long a) {
        return a * 10;
    }
    
    // Const overload
    int getValue() { return 1; }
    int getValue() const { return 2; }
};

// Default arguments with pointers
int sumArray(int* arr, int size, int multiplier = 1) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum * multiplier;
}

int main() {
    // Test function with default arguments
    if (add(1, 2, 3) != 6) return 1;      // All provided
    if (add(1, 2) != 23) return 2;        // c defaults to 20
    if (add(1) != 31) return 3;           // b=10, c=20
    
    // Test function overloading
    if (multiply(5) != 10) return 4;       // Single arg version
    if (multiply(3, 4) != 12) return 5;    // Two arg version
    if (multiply(2, 3, 4) != 24) return 6; // Three arg version
    
    // Test Rectangle constructor defaults
    Rectangle r1;              // Default 1x1
    if (r1.area() != 1) return 7;
    
    Rectangle r2(5);           // 5x1
    if (r2.area() != 5) return 8;
    
    Rectangle r3(4, 3);        // 4x3
    if (r3.area() != 12) return 9;
    
    // Test method with default argument
    Rectangle r4(2, 3);
    r4.scale();                // Default factor 2
    if (r4.area() != 24) return 10;  // (2*2) * (3*2) = 24
    
    Rectangle r5(2, 3);
    r5.scale(3);               // Factor 3
    if (r5.area() != 54) return 11;  // (2*3) * (3*3) = 54
    
    // Test resize with defaults
    Rectangle r6(1, 1);
    r6.resize();               // Both default to 10
    if (r6.area() != 100) return 12;
    
    r6.resize(5);              // Only width, height defaults to 10
    if (r6.area() != 50) return 13;
    
    r6.resize(4, 6);           // Both provided
    if (r6.area() != 24) return 14;
    
    // Test Calculator overloads
    Calculator calc;
    if (calc.compute(5) != 5) return 15;
    if (calc.compute(3, 4) != 7) return 16;
    if (calc.compute(1, 2, 3) != 6) return 17;
    if (calc.compute(10L) != 100L) return 18;  // long overload
    
    // Test const overload
    Calculator nonConstCalc;
    const Calculator constCalc;
    if (nonConstCalc.getValue() != 1) return 19;
    if (constCalc.getValue() != 2) return 20;
    
    // Test sumArray with default multiplier
    int arr[] = {1, 2, 3, 4, 5};
    if (sumArray(arr, 5) != 15) return 21;      // multiplier defaults to 1
    if (sumArray(arr, 5, 2) != 30) return 22;   // multiplier = 2
    
    return 0; // success
}
