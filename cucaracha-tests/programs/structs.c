// Test struct support in C
// Tests struct declaration, initialization, access, nesting, and passing

struct Point {
    int x;
    int y;
};

struct Rectangle {
    struct Point topLeft;
    struct Point bottomRight;
};

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct Nested {
    int id;
    struct {
        int width;
        int height;
    } dimensions;
    struct Color color;
};

// Struct with array member
struct Buffer {
    int size;
    int data[10];
};

// Struct with bitfields
struct Flags {
    unsigned int enabled : 1;
    unsigned int visible : 1;
    unsigned int selected : 1;
    unsigned int reserved : 5;
    unsigned int value : 8;
};

// Function taking struct by value
int point_distance_squared(struct Point p1, struct Point p2) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    return dx * dx + dy * dy;
}

// Function taking struct by pointer
void point_translate(struct Point* p, int dx, int dy) {
    p->x += dx;
    p->y += dy;
}

// Function returning struct
struct Point point_create(int x, int y) {
    struct Point p;
    p.x = x;
    p.y = y;
    return p;
}

// Function with nested struct parameter
int rectangle_area(struct Rectangle r) {
    int width = r.bottomRight.x - r.topLeft.x;
    int height = r.bottomRight.y - r.topLeft.y;
    return width * height;
}

// Function modifying nested struct via pointer
void rectangle_move(struct Rectangle* r, int dx, int dy) {
    r->topLeft.x += dx;
    r->topLeft.y += dy;
    r->bottomRight.x += dx;
    r->bottomRight.y += dy;
}

int main() {
    // Test basic struct initialization and access
    struct Point p1;
    p1.x = 10;
    p1.y = 20;
    if (p1.x != 10) return 1;
    if (p1.y != 20) return 2;
    
    // Test designated initializer style (C99)
    struct Point p2 = {30, 40};
    if (p2.x != 30) return 3;
    if (p2.y != 40) return 4;
    
    // Test struct assignment
    struct Point p3 = p2;
    if (p3.x != 30) return 5;
    if (p3.y != 40) return 6;
    
    // Test function returning struct
    struct Point p4 = point_create(50, 60);
    if (p4.x != 50) return 7;
    if (p4.y != 60) return 8;
    
    // Test function taking struct by value
    struct Point a = {0, 0};
    struct Point b = {3, 4};
    int dist_sq = point_distance_squared(a, b);
    if (dist_sq != 25) return 9;  // 3^2 + 4^2 = 25
    
    // Test function taking struct by pointer
    struct Point p5 = {100, 200};
    point_translate(&p5, 10, 20);
    if (p5.x != 110) return 10;
    if (p5.y != 220) return 11;
    
    // Test nested struct
    struct Rectangle rect;
    rect.topLeft.x = 0;
    rect.topLeft.y = 0;
    rect.bottomRight.x = 10;
    rect.bottomRight.y = 5;
    
    int area = rectangle_area(rect);
    if (area != 50) return 12;
    
    // Test nested struct modification via pointer
    rectangle_move(&rect, 5, 5);
    if (rect.topLeft.x != 5) return 13;
    if (rect.topLeft.y != 5) return 14;
    if (rect.bottomRight.x != 15) return 15;
    if (rect.bottomRight.y != 10) return 16;
    
    // Test struct with color (multi-byte members)
    struct Color color = {255, 128, 64, 255};
    if (color.r != 255) return 17;
    if (color.g != 128) return 18;
    if (color.b != 64) return 19;
    if (color.a != 255) return 20;
    
    // Test deeply nested struct
    struct Nested nested;
    nested.id = 42;
    nested.dimensions.width = 100;
    nested.dimensions.height = 50;
    nested.color.r = 200;
    nested.color.g = 100;
    nested.color.b = 50;
    nested.color.a = 255;
    
    if (nested.id != 42) return 21;
    if (nested.dimensions.width != 100) return 22;
    if (nested.dimensions.height != 50) return 23;
    if (nested.color.r != 200) return 24;
    
    // Test struct with array member
    struct Buffer buf;
    buf.size = 5;
    for (int i = 0; i < 5; i++) {
        buf.data[i] = i * 10;
    }
    
    if (buf.size != 5) return 25;
    if (buf.data[0] != 0) return 26;
    if (buf.data[2] != 20) return 27;
    if (buf.data[4] != 40) return 28;
    
    // Test bitfields
    struct Flags flags;
    flags.enabled = 1;
    flags.visible = 0;
    flags.selected = 1;
    flags.reserved = 0;
    flags.value = 127;
    
    if (flags.enabled != 1) return 29;
    if (flags.visible != 0) return 30;
    if (flags.selected != 1) return 31;
    if (flags.value != 127) return 32;
    
    // Test array of structs
    struct Point points[3];
    points[0].x = 1; points[0].y = 2;
    points[1].x = 3; points[1].y = 4;
    points[2].x = 5; points[2].y = 6;
    
    int sum_x = 0;
    for (int i = 0; i < 3; i++) {
        sum_x += points[i].x;
    }
    if (sum_x != 9) return 33;  // 1 + 3 + 5 = 9
    
    // Test pointer to struct member
    struct Point* ptr = &p1;
    if (ptr->x != 10) return 34;
    if (ptr->y != 20) return 35;
    
    ptr->x = 999;
    if (p1.x != 999) return 36;
    
    // Test sizeof struct
    if (sizeof(struct Point) < sizeof(int) * 2) return 37;
    if (sizeof(struct Color) < 4) return 38;
    
    return 0; // success
}
