// Test conditional statements
int main() {
    int x = 10;
    int y = 5;
    int result = 0;
    
    // If-else test
    if (x > y) {
        result += 1;
    } else {
        result -= 1;
    }
    
    // Switch-case test
    switch (y) {
        case 5:
            result += 10;
            break;
        case 10:
            result += 20;
            break;
        default:
            result += 5;
            break;
    }
    
    // Ternary operator test
    int ternary = (x == 10) ? 100 : 200;
    
    // Validate results: result=11, ternary=100
    if (result == 11 && ternary == 100) {
        return 0; // success
    }
    
    return 1; // failure
}