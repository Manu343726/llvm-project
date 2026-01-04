// Test C++ references - lvalue and rvalue references
// No stdlib dependencies

void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int& returnRef(int& x) {
    return x;
}

class RefHolder {
private:
    int& ref;

public:
    RefHolder(int& r) : ref(r) {}
    
    int get() const { return ref; }
    void set(int value) { ref = value; }
    void increment() { ref++; }
};

// Test const references
int sumConst(const int& a, const int& b) {
    return a + b;
}

// Modifying through reference
void addToValue(int& value, int amount) {
    value += amount;
}

// Array reference parameter
void processArray(int (&arr)[5]) {
    for (int i = 0; i < 5; i++) {
        arr[i] *= 2;
    }
}

int main() {
    // Test basic reference swap
    int a = 10, b = 20;
    swap(a, b);
    if (a != 20) return 1;
    if (b != 10) return 2;
    
    // Test returning reference
    int x = 100;
    int& ref = returnRef(x);
    if (ref != 100) return 3;
    
    ref = 200;
    if (x != 200) return 4;  // x should be modified through ref
    
    // Test reference in class
    int y = 50;
    RefHolder holder(y);
    if (holder.get() != 50) return 5;
    
    holder.set(60);
    if (y != 60) return 6;  // y should be modified
    
    holder.increment();
    if (y != 61) return 7;
    
    // Test const reference
    int c = 5, d = 7;
    if (sumConst(c, d) != 12) return 8;
    
    // Test addToValue
    int value = 100;
    addToValue(value, 25);
    if (value != 125) return 9;
    
    // Test array reference
    int arr[5] = {1, 2, 3, 4, 5};
    processArray(arr);
    if (arr[0] != 2) return 10;
    if (arr[1] != 4) return 11;
    if (arr[2] != 6) return 12;
    if (arr[3] != 8) return 13;
    if (arr[4] != 10) return 14;
    
    // Test reference initialization
    int original = 42;
    int& alias = original;
    alias = 99;
    if (original != 99) return 15;
    
    return 0; // success
}
