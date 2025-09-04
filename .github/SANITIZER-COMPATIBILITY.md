# Sanitizer Compatibility in ChainForge CI

## Issue Identified

The security stage was failing with the error:

```bash
cc1plus: error: '-fsanitize=thread' is incompatible with '-fsanitize=address'
```

## Root Cause

**Sanitizer Incompatibility**: The following sanitizers cannot be used together:

- ❌ **Address Sanitizer (`-fsanitize=address`)** + **Thread Sanitizer (`-fsanitize=thread`)**
- ❌ **Memory Sanitizer (`-fsanitize=memory`)** + **Address Sanitizer**
- ❌ **Memory Sanitizer** + **Thread Sanitizer**

**Why**: These sanitizers have conflicting memory management approaches and cannot coexist in the same build.

## Compatible Sanitizer Combinations

### ✅ **Safe Combinations**

1. **Address + Undefined Behavior**: `-fsanitize=address,undefined`
   - Address Sanitizer: Detects memory errors (buffer overflows, use-after-free)
   - Undefined Behavior Sanitizer: Catches undefined behavior (integer overflow, null pointer dereference)

2. **Thread Sanitizer**: `-fsanitize=thread` (standalone)
   - Detects data races and threading issues
   - Must be used separately from Address Sanitizer

3. **Memory Sanitizer**: `-fsanitize=memory` (standalone)
   - Detects uninitialized memory reads
   - Must be used separately from other sanitizers

## Fix Applied

### **Before (Incompatible)**

```yaml
- name: Build with sanitizers
  run: |
    cmake .. \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined,thread"  # ❌ Incompatible
```

### **After (Compatible)**

```yaml
# Stage 1: Address + Undefined Behavior Sanitizers
- name: Build with sanitizers
  run: |
    cmake .. \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"  # ✅ Compatible

# Stage 2: Thread Sanitizer (separate build)
- name: Build with thread sanitizer
  run: |
    cmake .. \
      -DCMAKE_CXX_FLAGS="-fsanitize=thread"  # ✅ Standalone
```

## Benefits of This Approach

1. **Compatibility**: No more compiler errors from conflicting sanitizers
2. **Comprehensive Coverage**: Still tests all three sanitizer types
3. **Separate Builds**: Each sanitizer gets its own clean build environment
4. **Better Debugging**: Clearer error messages when issues are found

## Sanitizer Coverage

### **Address Sanitizer (`-fsanitize=address`)**

- Buffer overflows
- Use-after-free
- Double-free
- Memory leaks
- Stack buffer overflows

### **Undefined Behavior Sanitizer (`-fsanitize=undefined`)**

- Integer overflow
- Null pointer dereference
- Division by zero
- Shift by negative amount
- Array bounds violations

### **Thread Sanitizer (`-fsanitize=thread`)**

- Data races
- Deadlocks
- Race conditions
- Threading bugs
- Lock ordering issues

## CI Pipeline Impact

The security stage now:

1. ✅ **Builds successfully** with Address + Undefined Behavior sanitizers
2. ✅ **Tests thread safety** with a separate Thread Sanitizer build
3. ✅ **Maintains coverage** of all three sanitizer types
4. ✅ **Provides clear feedback** on which sanitizer found issues

## Best Practices

1. **Never combine incompatible sanitizers** in the same build
2. **Use separate builds** for different sanitizer types
3. **Test all sanitizers** to ensure comprehensive coverage
4. **Monitor CI output** to catch sanitizer-specific issues

## Future Considerations

- **Memory Sanitizer**: Could add as a third separate build stage
- **Performance Impact**: Sanitizers add runtime overhead, consider release builds without them
- **Platform Support**: Some sanitizers may not be available on all platforms

This fix ensures that the CI pipeline can successfully test all sanitizer types while maintaining compatibility and build success.
