// Test static class members and methods
// No stdlib dependencies

class Counter {
private:
    int value;
    static int totalCount;
    static int instanceCount;

public:
    Counter() : value(0) {
        instanceCount++;
    }
    
    Counter(int initial) : value(initial) {
        instanceCount++;
        totalCount += initial;
    }
    
    ~Counter() {
        instanceCount--;
        totalCount -= value;
    }
    
    void increment() {
        value++;
        totalCount++;
    }
    
    void decrement() {
        value--;
        totalCount--;
    }
    
    int getValue() const { return value; }
    
    static int getTotalCount() { return totalCount; }
    static int getInstanceCount() { return instanceCount; }
    static void resetTotal() { totalCount = 0; }
};

// Static member initialization
int Counter::totalCount = 0;
int Counter::instanceCount = 0;

// Singleton pattern test
class Singleton {
private:
    static Singleton* instance;
    int data;
    
    Singleton() : data(0) {}

public:
    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
    
    static void destroyInstance() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }
    
    int getData() const { return data; }
    void setData(int d) { data = d; }
};

Singleton* Singleton::instance = nullptr;

// Class with static const members
class Config {
public:
    static const int MAX_SIZE = 100;
    static const int MIN_SIZE = 1;
    static const int DEFAULT_VALUE = 42;
    
    static int clamp(int value) {
        if (value < MIN_SIZE) return MIN_SIZE;
        if (value > MAX_SIZE) return MAX_SIZE;
        return value;
    }
};

int main() {
    // Test static counters
    if (Counter::getInstanceCount() != 0) return 1;
    if (Counter::getTotalCount() != 0) return 2;
    
    {
        Counter c1(10);
        if (Counter::getInstanceCount() != 1) return 3;
        if (Counter::getTotalCount() != 10) return 4;
        
        Counter c2(20);
        if (Counter::getInstanceCount() != 2) return 5;
        if (Counter::getTotalCount() != 30) return 6;
        
        c1.increment();
        if (Counter::getTotalCount() != 31) return 7;
        
        c2.decrement();
        if (Counter::getTotalCount() != 30) return 8;
    }
    
    // After scope, both counters destroyed
    if (Counter::getInstanceCount() != 0) return 9;
    // Note: totalCount should be 0 after destructors subtract values
    
    // Test singleton pattern
    Singleton* s1 = Singleton::getInstance();
    s1->setData(100);
    
    Singleton* s2 = Singleton::getInstance();
    if (s2->getData() != 100) return 10;  // Same instance
    
    s2->setData(200);
    if (s1->getData() != 200) return 11;  // Should reflect change
    
    // Verify same pointer
    if (s1 != s2) return 12;
    
    Singleton::destroyInstance();
    
    // Test static const members
    if (Config::MAX_SIZE != 100) return 13;
    if (Config::MIN_SIZE != 1) return 14;
    if (Config::DEFAULT_VALUE != 42) return 15;
    
    if (Config::clamp(50) != 50) return 16;
    if (Config::clamp(0) != 1) return 17;
    if (Config::clamp(150) != 100) return 18;
    
    return 0; // success
}
