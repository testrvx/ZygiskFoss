#pragma once

#include <cstdlib>
#include <cstring>
#include "misc.hpp"

struct SecureImpl {
    static void* allocate(size_t size) {
        return std::malloc(size);
    }

    static void deallocate(void* ptr, size_t size) {
        if (ptr != nullptr) {
            std::memset(ptr, 0, size);
            std::free(ptr);
        }
    }
};

template <typename T>
using SecureAllocator = stateless_allocator<T, SecureImpl>;

