// Test iterative factorial function
int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    int result = factorial(5); // Should be 120
    
    if (result == 120) {
        return 0; // success
    }
    
    return 1; // failure
}