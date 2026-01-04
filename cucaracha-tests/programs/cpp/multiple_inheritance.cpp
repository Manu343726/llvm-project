// Test multiple inheritance
// No stdlib dependencies

class Printable {
protected:
    int printId;

public:
    Printable() : printId(0) {}
    Printable(int id) : printId(id) {}
    
    int getPrintId() const { return printId; }
    int print() const { return printId * 10; }
};

class Serializable {
protected:
    int serialId;

public:
    Serializable() : serialId(0) {}
    Serializable(int id) : serialId(id) {}
    
    int getSerialId() const { return serialId; }
    int serialize() const { return serialId * 100; }
};

class Storable {
protected:
    int storeId;

public:
    Storable() : storeId(0) {}
    Storable(int id) : storeId(id) {}
    
    int getStoreId() const { return storeId; }
    int store() const { return storeId * 1000; }
};

// Multiple inheritance from two classes
class Document : public Printable, public Serializable {
private:
    int content;

public:
    Document() : Printable(1), Serializable(2), content(0) {}
    Document(int c) : Printable(1), Serializable(2), content(c) {}
    
    int getContent() const { return content; }
    
    int process() const {
        return print() + serialize() + content;
    }
};

// Multiple inheritance from three classes
class AdvancedDocument : public Printable, public Serializable, public Storable {
private:
    int data;

public:
    AdvancedDocument() : Printable(10), Serializable(20), Storable(30), data(0) {}
    AdvancedDocument(int d) : Printable(10), Serializable(20), Storable(30), data(d) {}
    
    int getData() const { return data; }
    
    int fullProcess() const {
        return print() + serialize() + store() + data;
    }
};

// Diamond problem - using virtual inheritance
class Base {
protected:
    int baseValue;

public:
    Base() : baseValue(100) {}
    Base(int v) : baseValue(v) {}
    
    int getBaseValue() const { return baseValue; }
    void setBaseValue(int v) { baseValue = v; }
};

class Left : virtual public Base {
protected:
    int leftValue;

public:
    Left() : Base(100), leftValue(10) {}
    Left(int lv) : Base(100), leftValue(lv) {}
    
    int getLeftValue() const { return leftValue; }
};

class Right : virtual public Base {
protected:
    int rightValue;

public:
    Right() : Base(100), rightValue(20) {}
    Right(int rv) : Base(100), rightValue(rv) {}
    
    int getRightValue() const { return rightValue; }
};

class Diamond : public Left, public Right {
private:
    int diamondValue;

public:
    Diamond() : Base(100), Left(10), Right(20), diamondValue(30) {}
    
    int getDiamondValue() const { return diamondValue; }
    
    int sum() const {
        return baseValue + leftValue + rightValue + diamondValue;
    }
};

int main() {
    // Test simple multiple inheritance
    Document doc(5);
    if (doc.getPrintId() != 1) return 1;
    if (doc.getSerialId() != 2) return 2;
    if (doc.getContent() != 5) return 3;
    if (doc.print() != 10) return 4;     // 1 * 10
    if (doc.serialize() != 200) return 5; // 2 * 100
    if (doc.process() != 215) return 6;  // 10 + 200 + 5
    
    // Test three-way multiple inheritance
    AdvancedDocument adoc(7);
    if (adoc.getPrintId() != 10) return 7;
    if (adoc.getSerialId() != 20) return 8;
    if (adoc.getStoreId() != 30) return 9;
    if (adoc.print() != 100) return 10;      // 10 * 10
    if (adoc.serialize() != 2000) return 11; // 20 * 100
    if (adoc.store() != 30000) return 12;    // 30 * 1000
    if (adoc.fullProcess() != 32107) return 13; // 100 + 2000 + 30000 + 7
    
    // Test diamond inheritance with virtual base
    Diamond d;
    if (d.getBaseValue() != 100) return 14;
    if (d.getLeftValue() != 10) return 15;
    if (d.getRightValue() != 20) return 16;
    if (d.getDiamondValue() != 30) return 17;
    if (d.sum() != 160) return 18;  // 100 + 10 + 20 + 30
    
    // Verify there's only one Base (not two copies)
    d.setBaseValue(200);
    if (d.getBaseValue() != 200) return 19;
    if (d.sum() != 260) return 20;  // 200 + 10 + 20 + 30
    
    return 0; // success
}
