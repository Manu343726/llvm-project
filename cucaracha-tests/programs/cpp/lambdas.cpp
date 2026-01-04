// Test lambda expressions (C++11)
// No stdlib dependencies

// Helper function that takes a function pointer
int applyBinaryOp(int a, int b, int (*op)(int, int)) {
    return op(a, b);
}

// Function object (functor) for comparison
class Multiplier {
private:
    int factor;

public:
    Multiplier(int f) : factor(f) {}
    
    int operator()(int x) const {
        return x * factor;
    }
};

// Simple function pointer type
typedef int (*IntUnaryFunc)(int);
typedef int (*IntBinaryFunc)(int, int);

int main() {
    // Test simple lambda stored in auto
    auto add = [](int a, int b) { return a + b; };
    if (add(3, 4) != 7) return 1;
    
    auto multiply = [](int a, int b) { return a * b; };
    if (multiply(3, 4) != 12) return 2;
    
    // Test lambda with capture by value
    int x = 10;
    auto addX = [x](int a) { return a + x; };
    if (addX(5) != 15) return 3;
    
    x = 20;  // Change x
    if (addX(5) != 15) return 4;  // Lambda captured old value
    
    // Test lambda with capture by reference
    int counter = 0;
    auto incrementCounter = [&counter]() { counter++; };
    
    incrementCounter();
    if (counter != 1) return 5;
    
    incrementCounter();
    incrementCounter();
    if (counter != 3) return 6;
    
    // Test lambda with mixed captures
    int a = 100;
    int b = 200;
    auto mixedCapture = [a, &b](int c) {
        b += c;  // Can modify b (captured by ref)
        return a + b + c;  // a is captured by value
    };
    
    int result = mixedCapture(50);
    // a=100, b was 200 now 250, c=50
    // returns 100 + 250 + 50 = 400
    if (result != 400) return 7;
    if (b != 250) return 8;  // b should be modified
    
    // Test mutable lambda (can modify value captures)
    int val = 5;
    auto mutableLambda = [val]() mutable {
        val++;
        return val;
    };
    
    if (mutableLambda() != 6) return 9;
    if (mutableLambda() != 7) return 10;  // Internal val increments
    if (val != 5) return 11;  // Original val unchanged
    
    // Test lambda with explicit return type
    auto divide = [](int x, int y) -> int {
        if (y == 0) return 0;
        return x / y;
    };
    if (divide(10, 2) != 5) return 12;
    if (divide(10, 0) != 0) return 13;
    
    // Test immediately invoked lambda
    int immediate = [](int x, int y) { return x * y + 1; }(3, 4);
    if (immediate != 13) return 14;  // 3*4+1 = 13
    
    // Test lambda stored as function pointer (non-capturing only)
    IntBinaryFunc subtract = [](int a, int b) { return a - b; };
    if (subtract(10, 3) != 7) return 15;
    
    // Test lambda with default capture (all by value)
    int p = 1, q = 2, r = 3;
    auto allByValue = [=]() { return p + q + r; };
    if (allByValue() != 6) return 16;
    
    // Test lambda with default capture (all by reference)
    int sum = 0;
    auto accumulate = [&](int n) { sum += n; };
    accumulate(10);
    accumulate(20);
    accumulate(30);
    if (sum != 60) return 17;
    
    // Compare with functor
    Multiplier mult3(3);
    if (mult3(5) != 15) return 18;
    
    auto lambdaMult3 = [](int x) { return x * 3; };
    if (lambdaMult3(5) != 15) return 19;
    
    return 0; // success
}
