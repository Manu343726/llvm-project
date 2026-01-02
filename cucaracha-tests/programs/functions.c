// Test function calls and parameter passing
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

void increment(int* ptr) {
    (*ptr)++;
}

int main() {
    int x = 5;
    int y = 3;
    
    int sum = add(x, y);           // 8
    int product = multiply(x, y);   // 15
    
    increment(&x);                 // x becomes 6
    
    // Validate results
    if (sum == 8 && product == 15 && x == 6) {
        return 0; // success
    }
    
    return 1; // failure
}