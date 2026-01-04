// Infinite recursion test - will overflow the stack
int recurse(int n) {
    return recurse(n + 1);
}

int main() {
    return recurse(0);
}
