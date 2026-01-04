// Test type casting operations in C++
// No stdlib dependencies

class Base {
protected:
    int value;

public:
    Base() : value(0) {}
    Base(int v) : value(v) {}
    virtual ~Base() {}
    
    virtual int getValue() const { return value; }
    virtual int getType() const { return 1; }
};

class Derived : public Base {
private:
    int extra;

public:
    Derived() : Base(), extra(0) {}
    Derived(int v, int e) : Base(v), extra(e) {}
    
    int getValue() const override { return value * 2; }
    int getType() const override { return 2; }
    int getExtra() const { return extra; }
};

class Unrelated {
public:
    int data;
    Unrelated() : data(999) {}
};

int main() {
    // Test static_cast - numeric conversions
    double d = 3.14159;
    int i = static_cast<int>(d);
    if (i != 3) return 1;
    
    int x = 42;
    double dx = static_cast<double>(x);
    // Can't easily test exact double equality, but conversion should work
    int back = static_cast<int>(dx);
    if (back != 42) return 2;
    
    // Test static_cast - pointer upcasting (derived to base)
    Derived der(10, 20);
    Base* basePtr = static_cast<Base*>(&der);
    if (basePtr->getValue() != 20) return 3;  // Should call Derived::getValue() = 10*2
    if (basePtr->getType() != 2) return 4;
    
    // Test static_cast - pointer downcasting (base to derived, when we know the actual type)
    Derived* derPtr = static_cast<Derived*>(basePtr);
    if (derPtr->getExtra() != 20) return 5;
    
    // Test const_cast
    const int constVal = 100;
    int* nonConstPtr = const_cast<int*>(&constVal);
    // Note: modifying constVal through nonConstPtr is undefined behavior
    // But we can test that the cast compiles and the pointer is valid
    if (*nonConstPtr != 100) return 6;
    
    // Test const_cast on reference
    const int& constRef = x;
    int& nonConstRef = const_cast<int&>(constRef);
    nonConstRef = 50;
    if (x != 50) return 7;
    
    // Test reinterpret_cast - pointer to integer
    int* ptr = &x;
    long ptrAsInt = reinterpret_cast<long>(ptr);
    int* ptrBack = reinterpret_cast<int*>(ptrAsInt);
    if (*ptrBack != 50) return 8;
    
    // Test reinterpret_cast - between pointer types
    int intVal = 0x12345678;
    char* charPtr = reinterpret_cast<char*>(&intVal);
    // Access first byte - value depends on endianness, but should work
    char firstByte = *charPtr;
    // Just verify we can do the cast and access
    (void)firstByte;
    
    // Test implicit conversion (no cast needed for upcasting)
    Derived d2(100, 200);
    Base& baseRef = d2;
    if (baseRef.getType() != 2) return 9;
    
    // Test C-style cast (should work like combination of casts)
    float f = 2.5f;
    int fromFloat = (int)f;
    if (fromFloat != 2) return 10;
    
    long longVal = 1000000L;
    int fromLong = (int)longVal;
    if (fromLong != 1000000) return 11;
    
    // Test casting with arithmetic
    int a = 7, b = 2;
    // Integer division
    int intDiv = a / b;
    if (intDiv != 3) return 12;
    
    // Floating point division via cast
    double floatDiv = static_cast<double>(a) / static_cast<double>(b);
    int floatDivInt = static_cast<int>(floatDiv * 10);  // Should be 35 (3.5 * 10)
    if (floatDivInt != 35) return 13;
    
    // Test pointer arithmetic with casts
    int arr[5] = {10, 20, 30, 40, 50};
    int* arrPtr = arr;
    char* bytePtr = reinterpret_cast<char*>(arrPtr);
    bytePtr += sizeof(int);  // Move one int forward
    int* nextIntPtr = reinterpret_cast<int*>(bytePtr);
    if (*nextIntPtr != 20) return 14;
    
    // Test void pointer casting
    void* voidPtr = static_cast<void*>(&x);
    int* intPtrFromVoid = static_cast<int*>(voidPtr);
    if (*intPtrFromVoid != 50) return 15;
    
    return 0; // success
}
