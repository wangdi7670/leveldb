#include <cassert>
#include <iostream>
#include <bitset>

#include <gtest/gtest.h>
#include "util/arena_my.h"
#include "util/random.h"

using namespace std;
using namespace leveldb;


TEST(ArenaMy, base) {
    ArenaMy arena;
    Random rnd(311);

    vector<std::pair<size_t, char *>> allocated;
    const int N = 10000;
    size_t bytes = 0;
    for (int i = 0; i < N; ++i) {
        size_t s = rnd.Uniform(6000);
        if (s == 0) {
            s = 1;
        }

        char *ptr = nullptr;
        if (rnd.OneIn(10)) {
            ptr = arena.AllocateAligned(s);
        } else {
            ptr = arena.Allocate(s);
        }

        bytes += s;
        allocated.push_back({s, ptr});

        for (size_t j = 0; j < s; ++j) {
            ptr[j] = i % 256;  // 2 ^ 8 = 256
            ASSERT_EQ(int(ptr[j]) & 0xff, i % 256);
        }

        ASSERT_GE(arena.MemoryUsage(), bytes);
    }

    ASSERT_EQ(N, allocated.size());
    for (int i = 0; i < allocated.size(); ++i) {
        size_t s = allocated.at(i).first;
        const char *ptr = allocated.at(i).second;

        for (int j = 0; j < s; ++j) {
            ASSERT_EQ(int(ptr[j]) & 0xff, i % 256); 
        }
    }
}
