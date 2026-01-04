// Test various member function types: const, static, inline
// No stdlib dependencies

class Calculator {
private:
    int accumulator;
    static int instance_count;

public:
    Calculator() : accumulator(0) {
        instance_count++;
    }
    
    ~Calculator() {
        instance_count--;
    }
    
    // Regular member functions
    void add(int n) { accumulator += n; }
    void subtract(int n) { accumulator -= n; }
    void multiply(int n) { accumulator *= n; }
    void clear() { accumulator = 0; }
    
    // Const member function
    int getResult() const { return accumulator; }
    
    // Static member functions
    static int getInstanceCount() { return instance_count; }
    
    static int staticAdd(int a, int b) {
        return a + b;
    }
    
    static int staticMultiply(int a, int b) {
        return a * b;
    }
    
    // Inline member function
    inline bool isZero() const { return accumulator == 0; }
    inline bool isPositive() const { return accumulator > 0; }
    inline bool isNegative() const { return accumulator < 0; }
};

int Calculator::instance_count = 0;

int main() {
    // Test static functions before creating any instance
    if (Calculator::getInstanceCount() != 0) return 1;
    if (Calculator::staticAdd(3, 4) != 7) return 2;
    if (Calculator::staticMultiply(3, 4) != 12) return 3;
    
    {
        Calculator calc;
        if (Calculator::getInstanceCount() != 1) return 4;
        
        // Test member functions
        calc.add(10);
        if (calc.getResult() != 10) return 5;
        
        calc.subtract(3);
        if (calc.getResult() != 7) return 6;
        
        calc.multiply(2);
        if (calc.getResult() != 14) return 7;
        
        // Test inline functions
        if (calc.isZero()) return 8;
        if (!calc.isPositive()) return 9;
        if (calc.isNegative()) return 10;
        
        calc.clear();
        if (!calc.isZero()) return 11;
        
        calc.subtract(5);
        if (!calc.isNegative()) return 12;
        if (calc.isPositive()) return 13;
        
        // Create another instance
        Calculator calc2;
        if (Calculator::getInstanceCount() != 2) return 14;
    }
    
    // Both calculators destroyed
    if (Calculator::getInstanceCount() != 0) return 15;
    
    return 0; // success
}
