// Test basic function and class templates
// No stdlib dependencies

// Simple function template
template<typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

template<typename T>
T min(T a, T b) {
    return (a < b) ? a : b;
}

// Function template with multiple type parameters
template<typename T, typename U>
T convert(U value) {
    return static_cast<T>(value);
}

// Class template
template<typename T>
class Box {
private:
    T content;

public:
    Box() : content() {}
    Box(T value) : content(value) {}
    
    T get() const { return content; }
    void set(T value) { content = value; }
    
    bool equals(const Box<T>& other) const {
        return content == other.content;
    }
};

// Class template with multiple parameters
template<typename T, int Size>
class StaticArray {
private:
    T data[Size];

public:
    StaticArray() {
        for (int i = 0; i < Size; i++) {
            data[i] = T();
        }
    }
    
    T& operator[](int index) {
        return data[index];
    }
    
    const T& operator[](int index) const {
        return data[index];
    }
    
    int size() const { return Size; }
    
    void fill(T value) {
        for (int i = 0; i < Size; i++) {
            data[i] = value;
        }
    }
    
    T sum() const {
        T result = T();
        for (int i = 0; i < Size; i++) {
            result = result + data[i];
        }
        return result;
    }
};

int main() {
    // Test function templates with int
    if (max(3, 5) != 5) return 1;
    if (min(3, 5) != 3) return 2;
    if (max(-10, -5) != -5) return 3;
    
    // Test function templates with different instantiation
    long la = 100L, lb = 200L;
    if (max(la, lb) != 200L) return 4;
    
    // Test convert template
    int i = convert<int>(3.14);
    if (i != 3) return 5;
    
    // Test Box template with int
    Box<int> intBox(42);
    if (intBox.get() != 42) return 6;
    
    intBox.set(100);
    if (intBox.get() != 100) return 7;
    
    Box<int> intBox2(100);
    if (!intBox.equals(intBox2)) return 8;
    
    // Test Box template with different type
    Box<long> longBox(1000000L);
    if (longBox.get() != 1000000L) return 9;
    
    // Test StaticArray template
    StaticArray<int, 5> arr;
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;
    arr[4] = 50;
    
    if (arr.size() != 5) return 10;
    if (arr[2] != 30) return 11;
    if (arr.sum() != 150) return 12;
    
    arr.fill(7);
    if (arr.sum() != 35) return 13;  // 7 * 5 = 35
    
    // Test with different size
    StaticArray<int, 10> arr10;
    if (arr10.size() != 10) return 14;
    
    return 0; // success
}
