#include <cassert>
#include "util/arena_my.h"

ArenaMy::ArenaMy() : allocatePtr_(nullptr), remaining_(0) {}

ArenaMy::~ArenaMy() {
    for (size_t i = 0; i < ptrs_.size(); ++i) {
        delete[] ptrs_.at(i);
    }
}

char *ArenaMy::Allocate(size_t bytes) {
    assert(bytes > 0);

    if (bytes < remaining_) {
        char *result = allocatePtr_;
        allocatePtr_ += bytes;
        remaining_ -= bytes;
        return result;
    }
    return AllocateNewBlock(bytes);
}

static size_t SIZE = 4096;

char *ArenaMy::AllocateNewBlock(size_t bytes) {
    if (bytes >= (SIZE / 4)) {
        char *result = new char[bytes];
        ptrs_.push_back(result);
        memoryUsage_.fetch_add(bytes + sizeof(char *), std::memory_order_relaxed);
        return result;
    }

    char *result = new char[SIZE];
    allocatePtr_ = result + bytes;
    remaining_ = SIZE - bytes;
    ptrs_.push_back(result);
    memoryUsage_.fetch_add(SIZE + sizeof(char*), std::memory_order_relaxed);
    return result;
}

char *ArenaMy::AllocateAligned(size_t bytes) {
    assert(bytes > 0);
    constexpr size_t align = sizeof(void *);
    static_assert((align & (align - 1)) == 0);

    size_t tail = reinterpret_cast<uintptr_t>(allocatePtr_) & (align - 1);
    size_t extra = (align - tail == 0) ? 0 : (align - tail);
    size_t needed = extra + bytes;

    if (remaining_ >= needed) {
        char *result = allocatePtr_ + extra;
        allocatePtr_ += needed;
        remaining_ -= needed;
        return result;
    }

    return AllocateNewBlock(bytes);
}