template <typename T>
PoolAllocator<T>::PoolAllocator(std::size_t pool_size): pool_size(pool_size), free(nullptr), available_memory(pool_size) 
{
    if (pool_size == 0) {
        throw std::invalid_argument("Pool size must be > 0");
    }

    std::size_t block_size = sizeof(T) > sizeof(void*) ? sizeof(T) : sizeof(void*);
    std::size_t total_size = block_size * pool_size;

    memory = ::operator new(total_size, std::align_val_t(alignof(T)));

    uint8_t* ptr = static_cast<uint8_t*>(memory);
    for (std::size_t i = 0; i < pool_size; ++i) {
        void* slot = ptr + i * block_size;
        *reinterpret_cast<void**>(slot) = (i + 1 < pool_size) ? (ptr + (i + 1) * block_size) : nullptr;
    }

    free = memory;
}

template <typename T>
PoolAllocator<T>::~PoolAllocator() {
    ::operator delete(memory, std::align_val_t(alignof(T)));
}

template <typename T>
T* PoolAllocator<T>::allocate() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!free) return nullptr;
    void* slot = free;
    free = *reinterpret_cast<void**>(slot);
    --available_memory;
    return static_cast<T*>(slot);
}

template <typename T>
void PoolAllocator<T>::destroy(T* ptr) {
    if (ptr) ptr->~T();
    ptr = nullptr;
}

template <typename T>
void PoolAllocator<T>::deallocate(T* ptr) {
    if (!ptr) return;
    destroy(ptr);
    std::lock_guard<std::mutex> lock(mtx);
    *reinterpret_cast<void**>(ptr) = free;
    free = ptr;
    ++available_memory;
}

template <typename T>
template <typename... Args>
T* PoolAllocator<T>::construct(Args&&... args) {
    T* mem = allocate();
    if (!mem) throw std::bad_alloc();
    return new (mem) T(std::forward<Args>(args)...);
}

template <typename T>
std::size_t PoolAllocator<T>::capacity() const {
    std::lock_guard<std::mutex> lock(mtx);
    return pool_size;
}

template <typename T>
std::size_t PoolAllocator<T>::available() const {
    std::lock_guard<std::mutex> lock(mtx);
    return available_memory;
}

template <typename T>
bool PoolAllocator<T>::is_full() const {
    std::lock_guard<std::mutex> lock(mtx);
    return available_memory == 0;
}

template <typename T>
bool PoolAllocator<T>::is_empty() const {
    std::lock_guard<std::mutex> lock(mtx);
    return available_memory == pool_size;
}


