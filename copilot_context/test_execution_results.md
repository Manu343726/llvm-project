# Test Execution Results

## Test Status: ALL TESTS PASSING ✅

**Last Updated:** January 2, 2026

### Summary
- Total Tests: 48 (8 programs × 6 phases each)
- Passed: 48 tests (100%)
- Failed: 0 tests (0%)
- Test Types: setup, llvm_ir, compile, binary, execute (.cucaracha), execute_binary (.o)

### Test Results Details

All tests now pass including execution tests:

| Program | Setup | LLVM IR | Compile | Binary | Execute | Execute Binary |
|---------|-------|---------|---------|--------|---------|----------------|
| hello_world | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| arithmetic | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| fibonacci | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| factorial | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| loops | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| conditionals | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| arrays | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| functions | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

---

### Fixes Applied (January 2, 2026)

#### 1. Arrays memcpy Constant Inlining Fix
- **Issue:** Arrays test failed - sum was 5 instead of expected 15
- **Root Cause:** LLVM's memcpy expansion loaded ONE constant and stored it to all array positions
- **Technical Detail:** `getMemsetStringVal()` returned empty SDValue, falling through to buggy load/store path
- **Fix:** Added `shouldConvertConstantLoadToIntImm()` override returning `true` in `CucarachaISelLowering.h`
- **Result:** LLVM now inlines constant array values as immediate stores

```cpp
// Added to CucarachaISelLowering.h
bool shouldConvertConstantLoadToIntImm(const APInt &Imm,
                                        Type *Ty) const override {
  return true;
}
```

---

### Fixes Applied (December 2024)

#### 1. Hardware Division Instructions
- **Issue:** Division operations required library calls
- **Fix:** Added `SDIV` and `UDIV` hardware instructions to `CucarachaInstrInfo.td`
- **Result:** Native division instructions are now generated

#### 2. Remainder Operations (srem/urem)
- **Issue:** `srem` instruction not supported, causing arithmetic test failure
- **Fix:** Added `SREM`, `UREM`, `SDIVREM`, `UDIVREM` to Expand list in `CucarachaISelLowering.cpp`
- **Result:** Remainder expands to div + mul + sub sequence

#### 3. SELECT_CC Conditional Select
- **Issue:** `select_cc` instruction not supported, causing conditionals test failure
- **Fix:** Implemented custom lowering for `SELECT_CC`:
  - Added `CucarachaISD::SELECT_CC` node type
  - Created `SELECT_CC` pseudo instruction with `usesCustomInserter`
  - Implemented `EmitInstrWithCustomInserter` to expand to CMP + Bcc + PHI
- **Files Modified:**
  - `CucarachaISelLowering.cpp/h` - Custom lowering and inserter
  - `CucarachaInstrInfo.td` - SELECT and SELECT_CC pseudo instructions  
  - `CucarachaOperators.td` - SDNode definitions

#### 4. Bug Fixes
- **Iterator Dereference:** Fixed `storeRegToStackSlot` and `loadRegFromStackSlot` in `CucarachaInstrInfo.cpp` to handle end iterators
- **Condition Code Printing:** Fixed `printCondCode` in `CucarachaInstPrinter.cpp` to use ARM-style condition codes

### Condition Code Mapping (ARM-style)
| Code | Value | Meaning |
|------|-------|---------|
| EQ | 0 | Equal |
| NE | 1 | Not Equal |
| HS | 2 | Unsigned >= |
| LO | 3 | Unsigned < |
| HI | 8 | Unsigned > |
| LS | 9 | Unsigned <= |
| GE | 10 | Signed >= |
| LT | 11 | Signed < |
| GT | 12 | Signed > |
| LE | 13 | Signed <= |

### Build & Test Commands
```bash
# Build clang
cd llvm-project/build_vs2022
cmake --build . --target clang --config Debug

# Run all cucaracha tests
ctest -C Debug -V
```
