# Cucaracha LLVM Backend Errors

## Overview
This document tracks specific LLVM backend code generation errors that need to be resolved for the Cucaracha target.

## Active Backend Errors

### Error #1: Signed Remainder Operation (srem)
- **Test Case:** [arithmetic.c](../cucaracha-tests/programs/arithmetic.c)
- **Error Message:** `fatal error: error in backend: Cannot select: t37: i32 = srem t35, t36`
- **Source Operation:** Modulo operator `%` in C code
- **LLVM IR:** `srem` instruction (signed remainder)
- **Location:** `CucarachaGenDAGISel.inc:400`
- **Status:** ❌ Not implemented
- **Required Fix:** Add TableGen instruction pattern for signed remainder operation

### Error #2: Conditional Select Operation (select_cc)  
- **Test Case:** [conditionals.c](../cucaracha-tests/programs/conditionals.c)
- **Error Message:** `fatal error: error in backend: Cannot select: t28: i32 = select_cc t4, Constant:i32<10>, Constant:i32<100>, Constant:i32<200>, seteq:ch`
- **Source Operation:** Conditional expressions, ternary operator, optimized if-else
- **LLVM IR:** `select_cc` instruction (conditional select with comparison)
- **Location:** `CucarachaGenDAGISel.inc:400`
- **Status:** ❌ Not implemented
- **Required Fix:** Add TableGen instruction pattern for conditional select operations

## Backend Implementation Status

### Working Instructions
✅ Basic arithmetic: add, sub, mul  
✅ Memory operations: load, store  
✅ Function calls  
✅ Control flow: branches, jumps  
✅ Array access  
✅ Loop constructs  

### Missing Instructions
❌ Signed remainder (`srem`)  
❌ Conditional select (`select_cc`)  
❌ Potentially other complex operations  

## Next Actions Required
1. **Examine Cucaracha TableGen files** - Look at instruction definitions in `.td` files
2. **Add srem pattern** - Define how modulo operations map to target instructions
3. **Add select_cc pattern** - Define how conditional selects map to target instructions
4. **Test incrementally** - Verify fixes with failing test cases

## Technical Notes
- All errors occur in the DAG-to-DAG instruction selection phase
- Generated file: `CucarachaGenDAGISel.inc` (auto-generated from TableGen)
- Source files likely in: `llvm/lib/Target/Cucaracha/*.td`
- Error handler: `SelectionDAGISel::CannotYetSelect()`