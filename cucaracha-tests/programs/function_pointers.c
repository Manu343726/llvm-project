// Test function pointer support in C
// Tests declaration, assignment, calling, arrays, and callbacks

// Simple function pointer types
typedef int (*BinaryOp)(int, int);
typedef int (*UnaryOp)(int);
typedef void (*VoidFunc)(void);
typedef int (*Comparator)(const void*, const void*);

// Basic arithmetic functions
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b != 0 ? a / b : 0; }

// Unary functions
int negate(int x) { return -x; }
int square(int x) { return x * x; }
int increment(int x) { return x + 1; }
int decrement(int x) { return x - 1; }

// Global counter for testing void functions
static int global_counter = 0;

void reset_counter(void) { global_counter = 0; }
void increment_counter(void) { global_counter++; }
void add_to_counter(int n) { global_counter += n; }
int get_counter(void) { return global_counter; }

// Higher-order function: apply binary operation
int apply_binary(BinaryOp op, int a, int b) {
    return op(a, b);
}

// Higher-order function: apply unary operation
int apply_unary(UnaryOp op, int x) {
    return op(x);
}

// Higher-order function: apply operation to array
void apply_to_array(int* arr, int size, UnaryOp op) {
    for (int i = 0; i < size; i++) {
        arr[i] = op(arr[i]);
    }
}

// Higher-order function: fold/reduce array
int fold_array(int* arr, int size, BinaryOp op, int initial) {
    int result = initial;
    for (int i = 0; i < size; i++) {
        result = op(result, arr[i]);
    }
    return result;
}

// Function returning function pointer
BinaryOp get_operation(int op_code) {
    switch (op_code) {
        case 0: return add;
        case 1: return subtract;
        case 2: return multiply;
        case 3: return divide;
        default: return add;
    }
}

// Struct containing function pointer
struct Calculator {
    BinaryOp operation;
    int last_result;
};

int calculator_compute(struct Calculator* calc, int a, int b) {
    calc->last_result = calc->operation(a, b);
    return calc->last_result;
}

// Callback pattern
typedef void (*Callback)(int result);

static int callback_received_value = -1;

void my_callback(int result) {
    callback_received_value = result;
}

void compute_with_callback(BinaryOp op, int a, int b, Callback cb) {
    int result = op(a, b);
    if (cb != 0) {
        cb(result);
    }
}

// Comparison functions for sorting-like operations
int compare_int_asc(const void* a, const void* b) {
    return *(const int*)a - *(const int*)b;
}

int compare_int_desc(const void* a, const void* b) {
    return *(const int*)b - *(const int*)a;
}

// Simple bubble sort using comparator
void bubble_sort(int* arr, int size, Comparator cmp) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (cmp(&arr[j], &arr[j + 1]) > 0) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    // Test basic function pointer declaration and call
    int (*fp)(int, int) = add;
    if (fp(3, 4) != 7) return 1;
    
    // Test reassignment
    fp = subtract;
    if (fp(10, 3) != 7) return 2;
    
    fp = multiply;
    if (fp(6, 7) != 42) return 3;
    
    // Test typedef'd function pointer
    BinaryOp op = divide;
    if (op(20, 4) != 5) return 4;
    
    // Test unary function pointer
    UnaryOp uop = negate;
    if (uop(5) != -5) return 5;
    
    uop = square;
    if (uop(7) != 49) return 6;
    
    // Test higher-order function with binary op
    if (apply_binary(add, 100, 200) != 300) return 7;
    if (apply_binary(multiply, 5, 6) != 30) return 8;
    
    // Test higher-order function with unary op
    if (apply_unary(increment, 99) != 100) return 9;
    if (apply_unary(decrement, 100) != 99) return 10;
    
    // Test void function pointer
    reset_counter();
    if (get_counter() != 0) return 11;
    
    VoidFunc vf = increment_counter;
    vf();
    vf();
    vf();
    if (get_counter() != 3) return 12;
    
    vf = reset_counter;
    vf();
    if (get_counter() != 0) return 13;
    
    // Test apply_to_array
    int arr1[5] = {1, 2, 3, 4, 5};
    apply_to_array(arr1, 5, square);
    if (arr1[0] != 1) return 14;
    if (arr1[1] != 4) return 15;
    if (arr1[2] != 9) return 16;
    if (arr1[3] != 16) return 17;
    if (arr1[4] != 25) return 18;
    
    // Test fold_array
    int arr2[4] = {1, 2, 3, 4};
    int sum = fold_array(arr2, 4, add, 0);
    if (sum != 10) return 19;  // 1 + 2 + 3 + 4 = 10
    
    int product = fold_array(arr2, 4, multiply, 1);
    if (product != 24) return 20;  // 1 * 2 * 3 * 4 = 24
    
    // Test function returning function pointer
    BinaryOp retrieved = get_operation(0);
    if (retrieved(5, 3) != 8) return 21;
    
    retrieved = get_operation(2);  // multiply
    if (retrieved(5, 3) != 15) return 22;
    
    // Test array of function pointers
    BinaryOp operations[4] = {add, subtract, multiply, divide};
    if (operations[0](10, 5) != 15) return 23;
    if (operations[1](10, 5) != 5) return 24;
    if (operations[2](10, 5) != 50) return 25;
    if (operations[3](10, 5) != 2) return 26;
    
    // Test struct with function pointer
    struct Calculator calc;
    calc.operation = add;
    calc.last_result = 0;
    
    calculator_compute(&calc, 100, 50);
    if (calc.last_result != 150) return 27;
    
    calc.operation = multiply;
    calculator_compute(&calc, 7, 8);
    if (calc.last_result != 56) return 28;
    
    // Test callback pattern
    callback_received_value = -1;
    compute_with_callback(add, 20, 30, my_callback);
    if (callback_received_value != 50) return 29;
    
    compute_with_callback(multiply, 6, 7, my_callback);
    if (callback_received_value != 42) return 30;
    
    // Test null callback (should not crash)
    compute_with_callback(add, 1, 2, 0);
    if (callback_received_value != 42) return 31;  // unchanged
    
    // Test comparator-based sorting
    int arr3[5] = {3, 1, 4, 1, 5};
    bubble_sort(arr3, 5, compare_int_asc);
    if (arr3[0] != 1) return 32;
    if (arr3[1] != 1) return 33;
    if (arr3[2] != 3) return 34;
    if (arr3[3] != 4) return 35;
    if (arr3[4] != 5) return 36;
    
    int arr4[5] = {3, 1, 4, 1, 5};
    bubble_sort(arr4, 5, compare_int_desc);
    if (arr4[0] != 5) return 37;
    if (arr4[1] != 4) return 38;
    if (arr4[2] != 3) return 39;
    if (arr4[3] != 1) return 40;
    if (arr4[4] != 1) return 41;
    
    // Test function pointer comparison
    BinaryOp op1 = add;
    BinaryOp op2 = add;
    BinaryOp op3 = subtract;
    
    if (op1 != op2) return 42;  // Same function
    if (op1 == op3) return 43;  // Different functions
    
    // Test calling through pointer to function pointer
    BinaryOp* pp = &op1;
    if ((*pp)(5, 3) != 8) return 44;
    
    *pp = multiply;
    if ((*pp)(5, 3) != 15) return 45;
    
    return 0; // success
}
