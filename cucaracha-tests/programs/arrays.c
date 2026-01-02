// Test array operations
int main() {
    int arr[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    
    // Calculate sum of array elements
    for (int i = 0; i < 5; i++) {
        sum += arr[i];
    }
    
    // Modify array elements
    arr[0] = 10;
    arr[4] = 50;
    
    int new_sum = 0;
    for (int i = 0; i < 5; i++) {
        new_sum += arr[i];
    }
    
    // Validate results: sum=15, new_sum=69 (10+2+3+4+50)
    if (sum == 15 && new_sum == 69) {
        return 0; // success
    }
    
    return 1; // failure
}