// Test new and delete operators
// No stdlib dependencies

class Simple {
public:
    int value;
    
    Simple() : value(0) {}
    Simple(int v) : value(v) {}
};

class Tracked {
private:
    int data;
    static int liveCount;

public:
    Tracked() : data(0) { liveCount++; }
    Tracked(int d) : data(d) { liveCount++; }
    ~Tracked() { liveCount--; }
    
    int getData() const { return data; }
    void setData(int d) { data = d; }
    
    static int getLiveCount() { return liveCount; }
};

int Tracked::liveCount = 0;

// Overloaded new/delete for a specific class
class CustomAlloc {
private:
    int value;
    static int allocCount;
    static int deallocCount;

public:
    CustomAlloc() : value(0) {}
    CustomAlloc(int v) : value(v) {}
    
    int getValue() const { return value; }
    
    // Custom new operator
    static void* operator new(unsigned long size) {
        allocCount++;
        // Use global new
        return ::operator new(size);
    }
    
    // Custom delete operator
    static void operator delete(void* ptr) {
        deallocCount++;
        ::operator delete(ptr);
    }
    
    static int getAllocCount() { return allocCount; }
    static int getDeallocCount() { return deallocCount; }
};

int CustomAlloc::allocCount = 0;
int CustomAlloc::deallocCount = 0;

int main() {
    // Test basic new/delete
    int* intPtr = new int(42);
    if (*intPtr != 42) return 1;
    delete intPtr;
    
    // Test object allocation
    Simple* simple = new Simple(100);
    if (simple->value != 100) return 2;
    delete simple;
    
    // Test tracked allocation/deallocation
    if (Tracked::getLiveCount() != 0) return 3;
    
    Tracked* t1 = new Tracked(10);
    if (Tracked::getLiveCount() != 1) return 4;
    if (t1->getData() != 10) return 5;
    
    Tracked* t2 = new Tracked(20);
    if (Tracked::getLiveCount() != 2) return 6;
    
    delete t1;
    if (Tracked::getLiveCount() != 1) return 7;
    
    delete t2;
    if (Tracked::getLiveCount() != 0) return 8;
    
    // Test array new/delete
    int* arr = new int[5];
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;
    arr[4] = 50;
    
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += arr[i];
    }
    if (sum != 150) return 9;
    delete[] arr;
    
    // Test array of objects
    Tracked* trackedArr = new Tracked[3];
    if (Tracked::getLiveCount() != 3) return 10;
    
    trackedArr[0].setData(100);
    trackedArr[1].setData(200);
    trackedArr[2].setData(300);
    
    if (trackedArr[1].getData() != 200) return 11;
    
    delete[] trackedArr;
    if (Tracked::getLiveCount() != 0) return 12;
    
    // Test custom allocator
    if (CustomAlloc::getAllocCount() != 0) return 13;
    
    CustomAlloc* ca1 = new CustomAlloc(1000);
    if (CustomAlloc::getAllocCount() != 1) return 14;
    if (ca1->getValue() != 1000) return 15;
    
    CustomAlloc* ca2 = new CustomAlloc(2000);
    if (CustomAlloc::getAllocCount() != 2) return 16;
    
    delete ca1;
    if (CustomAlloc::getDeallocCount() != 1) return 17;
    
    delete ca2;
    if (CustomAlloc::getDeallocCount() != 2) return 18;
    
    return 0; // success
}
