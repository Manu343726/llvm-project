// Test basic arithmetic operations
int main() {
    int a = 10;
    int b = 5;
    
    int sum = a + b;        // 15
    int diff = a - b;       // 5 
    int prod = a * b;       // 50
    int quot = a / b;       // 2
    int mod = a % b;        // 0
    
    // Simple validation
    if (sum == 15 && diff == 5 && prod == 50 && quot == 2 && mod == 0) {
        return 0; // success
    }
    
    return 1; // failure
}