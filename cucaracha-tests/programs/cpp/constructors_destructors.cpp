// Test constructor and destructor behavior including copy/move semantics
// No stdlib dependencies

// Global counter to track constructor/destructor calls
static int ctor_count = 0;
static int dtor_count = 0;
static int copy_count = 0;

class Tracked {
private:
    int value;
    int id;
    static int next_id;

public:
    Tracked() : value(0), id(next_id++) {
        ctor_count++;
    }
    
    Tracked(int v) : value(v), id(next_id++) {
        ctor_count++;
    }
    
    Tracked(const Tracked& other) : value(other.value), id(next_id++) {
        copy_count++;
        ctor_count++;
    }
    
    ~Tracked() {
        dtor_count++;
    }
    
    int getValue() const { return value; }
    int getId() const { return id; }
};

int Tracked::next_id = 0;

Tracked createTracked(int val) {
    Tracked t(val);
    return t;
}

int main() {
    // Reset counters
    ctor_count = 0;
    dtor_count = 0;
    copy_count = 0;
    
    {
        Tracked t1;          // default ctor
        Tracked t2(42);      // parameterized ctor
        
        if (t1.getValue() != 0) return 1;
        if (t2.getValue() != 42) return 2;
    }
    // t1 and t2 go out of scope, dtors should be called
    
    if (ctor_count < 2) return 3;  // at least 2 constructors
    if (dtor_count < 2) return 4;  // at least 2 destructors
    
    // Test copy constructor
    int prev_copy = copy_count;
    int prev_ctor = ctor_count;
    {
        Tracked t3(100);
        Tracked t4 = t3;     // copy constructor
        
        if (t4.getValue() != 100) return 5;
        if (copy_count <= prev_copy) return 6;  // copy should have been called
    }
    
    // Test function return
    {
        Tracked t5 = createTracked(200);
        if (t5.getValue() != 200) return 7;
    }
    
    return 0; // success
}
