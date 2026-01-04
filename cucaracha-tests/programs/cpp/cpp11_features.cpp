// Test C++11 features: nullptr, auto, range-for, enum class
// No stdlib dependencies

// Enum class (scoped enumeration)
enum class Color {
    Red = 1,
    Green = 2,
    Blue = 3
};

enum class Status {
    Ok = 0,
    Error = 1,
    Pending = 2
};

// Class using nullptr
class OptionalInt {
private:
    int* ptr;

public:
    OptionalInt() : ptr(nullptr) {}
    
    OptionalInt(int value) {
        ptr = new int(value);
    }
    
    ~OptionalInt() {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
    
    bool hasValue() const { return ptr != nullptr; }
    
    int getValue() const {
        if (ptr != nullptr) {
            return *ptr;
        }
        return -1;  // Default value
    }
    
    void setValue(int value) {
        if (ptr == nullptr) {
            ptr = new int(value);
        } else {
            *ptr = value;
        }
    }
    
    void clear() {
        if (ptr != nullptr) {
            delete ptr;
            ptr = nullptr;
        }
    }
};

// Test auto with simple types
int getInt() { return 42; }
long getLong() { return 1000000L; }

// Constexpr function (C++11)
constexpr int square(int x) {
    return x * x;
}

constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

// Test uniform initialization
struct Point {
    int x;
    int y;
};

class InitTestClass {
public:
    int a;
    int b;
    
    InitTestClass(int a, int b) : a{a}, b{b} {}
};

int main() {
    // Test enum class
    Color c1 = Color::Red;
    Color c2 = Color::Blue;
    
    if (static_cast<int>(c1) != 1) return 1;
    if (static_cast<int>(c2) != 3) return 2;
    
    Status s = Status::Ok;
    if (static_cast<int>(s) != 0) return 3;
    
    // Test nullptr
    int* nullPtr = nullptr;
    if (nullPtr != nullptr) return 4;
    
    int value = 42;
    int* validPtr = &value;
    if (validPtr == nullptr) return 5;
    
    // Test OptionalInt with nullptr
    OptionalInt opt1;
    if (opt1.hasValue()) return 6;
    
    OptionalInt opt2(100);
    if (!opt2.hasValue()) return 7;
    if (opt2.getValue() != 100) return 8;
    
    opt1.setValue(50);
    if (!opt1.hasValue()) return 9;
    if (opt1.getValue() != 50) return 10;
    
    opt1.clear();
    if (opt1.hasValue()) return 11;
    
    // Test auto
    auto i = getInt();
    if (i != 42) return 12;
    
    auto l = getLong();
    if (l != 1000000L) return 13;
    
    auto x = 5;
    auto y = x * 2;
    if (y != 10) return 14;
    
    // Test constexpr
    constexpr int sq = square(5);
    if (sq != 25) return 15;
    
    constexpr int fact5 = factorial(5);
    if (fact5 != 120) return 16;
    
    // Runtime constexpr call
    int runtime_sq = square(6);
    if (runtime_sq != 36) return 17;
    
    // Test uniform initialization
    Point p1{10, 20};
    if (p1.x != 10) return 18;
    if (p1.y != 20) return 19;
    
    InitTestClass tc{100, 200};
    if (tc.a != 100) return 20;
    if (tc.b != 200) return 21;
    
    // Test array with uniform initialization
    int arr[5]{1, 2, 3, 4, 5};
    if (arr[0] != 1) return 22;
    if (arr[4] != 5) return 23;
    
    return 0; // success
}
