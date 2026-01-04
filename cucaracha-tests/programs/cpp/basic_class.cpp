// Test basic class definition, constructors, destructors, and methods
// No stdlib dependencies

class Counter {
private:
    int value;

public:
    Counter() : value(0) {}
    Counter(int initial) : value(initial) {}
    
    void increment() { value++; }
    void decrement() { value--; }
    void add(int n) { value += n; }
    int get() const { return value; }
    void set(int v) { value = v; }
};

int main() {
    Counter c1;           // default constructor
    Counter c2(10);       // parameterized constructor
    
    if (c1.get() != 0) return 1;
    if (c2.get() != 10) return 2;
    
    c1.increment();
    if (c1.get() != 1) return 3;
    
    c1.add(5);
    if (c1.get() != 6) return 4;
    
    c1.decrement();
    if (c1.get() != 5) return 5;
    
    c1.set(100);
    if (c1.get() != 100) return 6;
    
    return 0; // success
}
