// Test template specialization
// No stdlib dependencies

// Primary template
template<typename T>
class TypeTraits {
public:
    static int getTypeId() { return 0; }  // Unknown type
    static int getSize() { return sizeof(T); }
    static bool isIntegral() { return false; }
};

// Explicit specialization for int
template<>
class TypeTraits<int> {
public:
    static int getTypeId() { return 1; }
    static int getSize() { return sizeof(int); }
    static bool isIntegral() { return true; }
};

// Explicit specialization for long
template<>
class TypeTraits<long> {
public:
    static int getTypeId() { return 2; }
    static int getSize() { return sizeof(long); }
    static bool isIntegral() { return true; }
};

// Explicit specialization for char
template<>
class TypeTraits<char> {
public:
    static int getTypeId() { return 3; }
    static int getSize() { return sizeof(char); }
    static bool isIntegral() { return true; }
};

// Explicit specialization for pointer types (partial-like via full specialization)
template<>
class TypeTraits<int*> {
public:
    static int getTypeId() { return 10; }
    static int getSize() { return sizeof(int*); }
    static bool isIntegral() { return false; }
};

// Function template with specialization
template<typename T>
T identity(T value) {
    return value;
}

// Specialization for char* - adds null check
template<>
char* identity<char*>(char* value) {
    if (value == nullptr) {
        static char empty[] = "";
        return empty;
    }
    return value;
}

// Template class with multiple parameters
template<typename T, typename U>
class Pair {
public:
    T first;
    U second;
    
    Pair() : first(), second() {}
    Pair(T f, U s) : first(f), second(s) {}
    
    T getFirst() const { return first; }
    U getSecond() const { return second; }
};

// Partial specialization - both types the same
template<typename T>
class Pair<T, T> {
public:
    T first;
    T second;
    
    Pair() : first(), second() {}
    Pair(T f, T s) : first(f), second(s) {}
    
    T getFirst() const { return first; }
    T getSecond() const { return second; }
    
    // Additional method only for same-type pairs
    T sum() const { return first + second; }
};

// Template with non-type parameter specialization
template<int N>
class Factorial {
public:
    static const int value = N * Factorial<N - 1>::value;
};

template<>
class Factorial<0> {
public:
    static const int value = 1;
};

template<>
class Factorial<1> {
public:
    static const int value = 1;
};

// Fibonacci via template
template<int N>
class Fibonacci {
public:
    static const int value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template<>
class Fibonacci<0> {
public:
    static const int value = 0;
};

template<>
class Fibonacci<1> {
public:
    static const int value = 1;
};

struct CustomType {
    int x;
};

int main() {
    // Test TypeTraits specializations
    if (TypeTraits<int>::getTypeId() != 1) return 1;
    if (TypeTraits<long>::getTypeId() != 2) return 2;
    if (TypeTraits<char>::getTypeId() != 3) return 3;
    if (TypeTraits<CustomType>::getTypeId() != 0) return 4;  // Unknown
    
    if (!TypeTraits<int>::isIntegral()) return 5;
    if (!TypeTraits<long>::isIntegral()) return 6;
    if (TypeTraits<CustomType>::isIntegral()) return 7;
    
    if (TypeTraits<int*>::getTypeId() != 10) return 8;
    if (TypeTraits<int*>::isIntegral()) return 9;
    
    // Test function template
    if (identity(42) != 42) return 10;
    if (identity(100L) != 100L) return 11;
    
    char str[] = "hello";
    if (identity(str) != str) return 12;
    
    // Test Pair with different types
    Pair<int, long> mixedPair(10, 20L);
    if (mixedPair.getFirst() != 10) return 13;
    if (mixedPair.getSecond() != 20L) return 14;
    
    // Test Pair partial specialization (same types)
    Pair<int, int> samePair(30, 40);
    if (samePair.getFirst() != 30) return 15;
    if (samePair.getSecond() != 40) return 16;
    if (samePair.sum() != 70) return 17;  // Only available for same-type pairs
    
    // Test compile-time Factorial
    if (Factorial<0>::value != 1) return 18;
    if (Factorial<1>::value != 1) return 19;
    if (Factorial<5>::value != 120) return 20;
    if (Factorial<6>::value != 720) return 21;
    
    // Test compile-time Fibonacci
    if (Fibonacci<0>::value != 0) return 22;
    if (Fibonacci<1>::value != 1) return 23;
    if (Fibonacci<5>::value != 5) return 24;   // 0,1,1,2,3,5
    if (Fibonacci<10>::value != 55) return 25; // 0,1,1,2,3,5,8,13,21,34,55
    
    return 0; // success
}
