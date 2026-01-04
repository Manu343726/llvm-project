// Test atomic operations - Cucaracha has atomic load and store
// No stdlib dependencies, uses compiler builtins

// Memory ordering constants (matching std::memory_order)
enum memory_order {
    memory_order_relaxed = 0,
    memory_order_consume = 1,
    memory_order_acquire = 2,
    memory_order_release = 3,
    memory_order_acq_rel = 4,
    memory_order_seq_cst = 5
};

// Simple atomic wrapper using builtins
template<typename T>
class Atomic {
private:
    volatile T value;

public:
    Atomic() : value(T()) {}
    Atomic(T initial) : value(initial) {}
    
    // Atomic load
    T load(memory_order order = memory_order_seq_cst) const {
        return __atomic_load_n(&value, order);
    }
    
    // Atomic store
    void store(T desired, memory_order order = memory_order_seq_cst) {
        __atomic_store_n(&value, desired, order);
    }
    
    // Atomic exchange
    T exchange(T desired, memory_order order = memory_order_seq_cst) {
        return __atomic_exchange_n(&value, desired, order);
    }
    
    // Atomic compare and exchange (strong)
    bool compare_exchange_strong(T& expected, T desired,
                                  memory_order success = memory_order_seq_cst,
                                  memory_order failure = memory_order_seq_cst) {
        return __atomic_compare_exchange_n(&value, &expected, desired,
                                           false, success, failure);
    }
    
    // Atomic compare and exchange (weak)
    bool compare_exchange_weak(T& expected, T desired,
                                memory_order success = memory_order_seq_cst,
                                memory_order failure = memory_order_seq_cst) {
        return __atomic_compare_exchange_n(&value, &expected, desired,
                                           true, success, failure);
    }
    
    // Atomic fetch-and-add
    T fetch_add(T arg, memory_order order = memory_order_seq_cst) {
        return __atomic_fetch_add(&value, arg, order);
    }
    
    // Atomic fetch-and-subtract
    T fetch_sub(T arg, memory_order order = memory_order_seq_cst) {
        return __atomic_fetch_sub(&value, arg, order);
    }
    
    // Atomic fetch-and-and
    T fetch_and(T arg, memory_order order = memory_order_seq_cst) {
        return __atomic_fetch_and(&value, arg, order);
    }
    
    // Atomic fetch-and-or
    T fetch_or(T arg, memory_order order = memory_order_seq_cst) {
        return __atomic_fetch_or(&value, arg, order);
    }
    
    // Atomic fetch-and-xor
    T fetch_xor(T arg, memory_order order = memory_order_seq_cst) {
        return __atomic_fetch_xor(&value, arg, order);
    }
    
    // Convenience operators
    operator T() const { return load(); }
    T operator=(T desired) { store(desired); return desired; }
    T operator++() { return fetch_add(1) + 1; }
    T operator++(int) { return fetch_add(1); }
    T operator--() { return fetch_sub(1) - 1; }
    T operator--(int) { return fetch_sub(1); }
    T operator+=(T arg) { return fetch_add(arg) + arg; }
    T operator-=(T arg) { return fetch_sub(arg) - arg; }
};

// Test atomic flag using builtin
class AtomicFlag {
private:
    volatile unsigned char flag;

public:
    AtomicFlag() : flag(0) {}
    
    bool test_and_set(memory_order order = memory_order_seq_cst) {
        return __atomic_test_and_set(&flag, order);
    }
    
    void clear(memory_order order = memory_order_seq_cst) {
        __atomic_clear(&flag, order);
    }
};

// Memory fence
inline void atomic_thread_fence(memory_order order) {
    __atomic_thread_fence(order);
}

inline void atomic_signal_fence(memory_order order) {
    __atomic_signal_fence(order);
}

int main() {
    // Test basic atomic int operations
    Atomic<int> counter(0);
    
    if (counter.load() != 0) return 1;
    
    counter.store(42);
    if (counter.load() != 42) return 2;
    
    // Test with different memory orderings
    counter.store(100, memory_order_relaxed);
    if (counter.load(memory_order_relaxed) != 100) return 3;
    
    counter.store(200, memory_order_release);
    if (counter.load(memory_order_acquire) != 200) return 4;
    
    // Test exchange
    int old = counter.exchange(300);
    if (old != 200) return 5;
    if (counter.load() != 300) return 6;
    
    // Test fetch_add
    counter.store(10);
    old = counter.fetch_add(5);
    if (old != 10) return 7;
    if (counter.load() != 15) return 8;
    
    // Test fetch_sub
    old = counter.fetch_sub(3);
    if (old != 15) return 9;
    if (counter.load() != 12) return 10;
    
    // Test fetch_and
    counter.store(0xFF);
    old = counter.fetch_and(0x0F);
    if (old != 0xFF) return 11;
    if (counter.load() != 0x0F) return 12;
    
    // Test fetch_or
    counter.store(0x0F);
    old = counter.fetch_or(0xF0);
    if (old != 0x0F) return 13;
    if (counter.load() != 0xFF) return 14;
    
    // Test fetch_xor
    counter.store(0xFF);
    old = counter.fetch_xor(0x0F);
    if (old != 0xFF) return 15;
    if (counter.load() != 0xF0) return 16;
    
    // Test compare_exchange_strong - success case
    counter.store(50);
    int expected = 50;
    bool success = counter.compare_exchange_strong(expected, 60);
    if (!success) return 17;
    if (counter.load() != 60) return 18;
    if (expected != 50) return 19;  // expected unchanged on success
    
    // Test compare_exchange_strong - failure case
    expected = 100;  // Wrong expected value
    success = counter.compare_exchange_strong(expected, 70);
    if (success) return 20;  // Should fail
    if (counter.load() != 60) return 21;  // Value unchanged
    if (expected != 60) return 22;  // expected updated to actual value
    
    // Test operator overloads
    Atomic<int> val(0);
    
    val = 10;
    if (val != 10) return 23;
    
    int pre = ++val;
    if (pre != 11) return 24;
    if (val != 11) return 25;
    
    int post = val++;
    if (post != 11) return 26;
    if (val != 12) return 27;
    
    pre = --val;
    if (pre != 11) return 28;
    
    post = val--;
    if (post != 11) return 29;
    if (val != 10) return 30;
    
    val += 5;
    if (val != 15) return 31;
    
    val -= 3;
    if (val != 12) return 32;
    
    // Test AtomicFlag (spinlock-like primitive)
    AtomicFlag flag;
    
    // First test_and_set should return false (was clear)
    if (flag.test_and_set()) return 33;
    
    // Second test_and_set should return true (was set)
    if (!flag.test_and_set()) return 34;
    
    // Clear and verify
    flag.clear();
    if (flag.test_and_set()) return 35;  // Should be clear now
    
    // Test atomic with different sizes
    Atomic<char> charAtom('A');
    if (charAtom.load() != 'A') return 36;
    charAtom.store('B');
    if (charAtom.load() != 'B') return 37;
    
    Atomic<short> shortAtom(1000);
    if (shortAtom.load() != 1000) return 38;
    shortAtom.fetch_add(234);
    if (shortAtom.load() != 1234) return 39;
    
    Atomic<long> longAtom(1000000L);
    if (longAtom.load() != 1000000L) return 40;
    longAtom.store(2000000L);
    if (longAtom.load() != 2000000L) return 41;
    
    // Test memory fences (just verify they compile and don't crash)
    atomic_thread_fence(memory_order_acquire);
    atomic_thread_fence(memory_order_release);
    atomic_thread_fence(memory_order_acq_rel);
    atomic_thread_fence(memory_order_seq_cst);
    atomic_signal_fence(memory_order_seq_cst);
    
    // Test pointer atomics
    int array[3] = {10, 20, 30};
    Atomic<int*> atomicPtr(array);
    
    if (*atomicPtr.load() != 10) return 42;
    
    atomicPtr.store(&array[1]);
    if (*atomicPtr.load() != 20) return 43;
    
    int* oldPtr = atomicPtr.exchange(&array[2]);
    if (*oldPtr != 20) return 44;
    if (*atomicPtr.load() != 30) return 45;
    
    return 0; // success
}
