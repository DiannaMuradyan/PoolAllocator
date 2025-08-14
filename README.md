# PoolAllocator (Thread-Safe)

A fixed-size, thread-safe memory pool for C++.  
Pre-allocates a single contiguous block and reuses it for fast, low-fragmentation allocations.

---

## Features
- Thread-safe (`std::mutex`)
- Placement new for in-place construction
- Constant-time allocate/deallocate via free list
- Works with POD & non-POD types
- O(1) operations, minimal heap usage

---

## Example
```cpp
#include <iostream>
#include "PoolAllocator.hpp"

struct MyObject {
    MyObject(int a, int b) { std::cout << "Constructed\n"; }
    ~MyObject() { std::cout << "Destroyed\n"; }
};

int main() {
    PoolAllocator<MyObject> pool(2);
    auto* a = pool.construct(1, 2);
    pool.deallocate(a);
}
