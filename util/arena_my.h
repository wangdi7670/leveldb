#include <vector>
#include <cstdint>
#include <cstddef>
#include <atomic>

using namespace std;

class ArenaMy {
public:
    ArenaMy();
    ~ArenaMy();

    char *Allocate(size_t bytes);

    char *AllocateAligned(size_t bytes);

    size_t MemoryUsage() const {
        return memoryUsage_.load(std::memory_order_relaxed);
    }

private:
    char *AllocateNewBlock(size_t bytes);

    char *allocatePtr_;
    size_t remaining_;
    std::vector<char *> ptrs_;
    std::atomic<size_t> memoryUsage_;
};