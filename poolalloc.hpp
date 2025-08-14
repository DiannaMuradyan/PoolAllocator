#pragma once
#include <cstddef>
#include <cstdint>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <mutex>
#include <utility>

template <typename T>
class PoolAllocator {
public:
    explicit PoolAllocator(std::size_t pool_size);
    ~PoolAllocator();

    T* allocate();
    void deallocate(T* ptr);
    void destroy (T *ptr);

    template <typename... Args>
    T* construct(Args&&... args);

    std::size_t capacity() const;
    std::size_t available() const;
    bool is_full() const;
    bool is_empty() const;

private:
    std::size_t pool_size;
    void* memory;
    void* free;
    std::size_t available_memory;
    mutable std::mutex mtx;
};

#include "poolalloc.cpp"