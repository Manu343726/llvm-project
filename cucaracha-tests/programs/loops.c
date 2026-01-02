// Test different loop constructs
int main() {
    int sum = 0;
    
    // For loop
    for (int i = 1; i <= 10; i++) {
        sum += i;
    }
    
    // While loop
    int j = 1;
    int prod = 1;
    while (j <= 5) {
        prod *= j;
        j++;
    }
    
    // Do-while loop
    int count = 0;
    do {
        count++;
    } while (count < 3);
    
    // Validate results: sum=55, prod=120, count=3
    if (sum == 55 && prod == 120 && count == 3) {
        return 0; // success
    }
    
    return 1; // failure
}