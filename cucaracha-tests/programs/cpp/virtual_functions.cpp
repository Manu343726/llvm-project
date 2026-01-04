// Test virtual functions and polymorphism
// No stdlib dependencies

class Animal {
protected:
    int age;

public:
    Animal() : age(0) {}
    Animal(int age) : age(age) {}
    virtual ~Animal() {}
    
    virtual int speak() const { return 0; }  // Base returns 0
    virtual int move() const { return 1; }   // Base returns 1
    
    int getAge() const { return age; }
    void setAge(int a) { age = a; }
};

class Dog : public Animal {
public:
    Dog() : Animal() {}
    Dog(int age) : Animal(age) {}
    
    int speak() const override { return 10; }  // Dog returns 10
    int move() const override { return 11; }   // Dog returns 11
    
    int fetch() const { return 100; }
};

class Cat : public Animal {
public:
    Cat() : Animal() {}
    Cat(int age) : Animal(age) {}
    
    int speak() const override { return 20; }  // Cat returns 20
    int move() const override { return 21; }   // Cat returns 21
    
    int scratch() const { return 200; }
};

class Bird : public Animal {
public:
    Bird() : Animal() {}
    Bird(int age) : Animal(age) {}
    
    int speak() const override { return 30; }  // Bird returns 30
    // move() not overridden, uses base class implementation
    
    int fly() const { return 300; }
};

// Pure virtual function test
class Shape {
public:
    virtual ~Shape() {}
    virtual int area() const = 0;  // Pure virtual
    virtual int perimeter() const = 0;  // Pure virtual
};

class Circle : public Shape {
    int radius;
public:
    Circle(int r) : radius(r) {}
    
    int area() const override { return 3 * radius * radius; }  // Approximation
    int perimeter() const override { return 6 * radius; }  // Approximation
};

class Rectangle : public Shape {
    int width, height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    
    int area() const override { return width * height; }
    int perimeter() const override { return 2 * (width + height); }
};

// Function taking base class pointer
int makeSpeak(const Animal* animal) {
    return animal->speak();
}

int makeMove(const Animal* animal) {
    return animal->move();
}

int main() {
    // Test polymorphism through base pointer
    Dog dog(3);
    Cat cat(5);
    Bird bird(2);
    
    Animal* animals[3];
    animals[0] = &dog;
    animals[1] = &cat;
    animals[2] = &bird;
    
    // Test virtual speak()
    if (animals[0]->speak() != 10) return 1;  // Dog
    if (animals[1]->speak() != 20) return 2;  // Cat
    if (animals[2]->speak() != 30) return 3;  // Bird
    
    // Test virtual move()
    if (animals[0]->move() != 11) return 4;  // Dog
    if (animals[1]->move() != 21) return 5;  // Cat
    if (animals[2]->move() != 1) return 6;   // Bird uses base class
    
    // Test through function calls
    if (makeSpeak(&dog) != 10) return 7;
    if (makeSpeak(&cat) != 20) return 8;
    if (makeMove(&bird) != 1) return 9;
    
    // Test non-virtual functions
    if (dog.fetch() != 100) return 10;
    if (cat.scratch() != 200) return 11;
    if (bird.fly() != 300) return 12;
    
    // Test ages
    if (dog.getAge() != 3) return 13;
    if (cat.getAge() != 5) return 14;
    
    // Test pure virtual with concrete implementations
    Circle circle(10);
    Rectangle rect(4, 5);
    
    Shape* shapes[2];
    shapes[0] = &circle;
    shapes[1] = &rect;
    
    if (shapes[0]->area() != 300) return 15;  // 3 * 10 * 10
    if (shapes[1]->area() != 20) return 16;   // 4 * 5
    
    if (shapes[0]->perimeter() != 60) return 17;  // 6 * 10
    if (shapes[1]->perimeter() != 18) return 18;  // 2 * (4 + 5)
    
    return 0; // success
}
