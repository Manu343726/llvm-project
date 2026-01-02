// Test recursive fibonacci function
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
    int result = fibonacci(8); // Should be 21
    
    if (result == 21) {
        return 0; // success
    }
    
    return 1; // failure
}