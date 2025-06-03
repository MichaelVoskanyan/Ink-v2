// =================================================================================================
// This file exists solely to test the memory.h header file and compare it's speed to the C++ STL
// implementation of unique and shared pointers.
// =================================================================================================

#include "ink_memory.h"
#include <memory>
#include <cstdio>
#include <chrono>

struct BigCyka {
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
    uint64_t e;
    uint64_t f;
    uint64_t g;
    char name[1024];

    BigCyka() {
        printf("Creating BigCyka!\n");
    }

    ~BigCyka() {
        printf("Deleting BigCyka!!\n");
    }
};

void testUnique() {
    printf("Beginning tests...\n");
    auto t1 = std::chrono::high_resolution_clock::now();
    std::unique_ptr<BigCyka> m_cyka = std::make_unique<BigCyka>();
    std::unique_ptr<BigCyka> m_twoCyka = std::move(m_cyka);
    m_twoCyka.reset();
    auto t2 = std::chrono::high_resolution_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("STD Unique time: %ld\n", d);

    t1 = std::chrono::high_resolution_clock::now();
    UniquePtr<BigCyka> mycyka(new BigCyka());
    UniquePtr<BigCyka> mytwocyka = std::move(mycyka);
    mytwocyka.reset();
    t2 = std::chrono::high_resolution_clock::now();
    d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("My Unique time: %ld\n", d);
}

void testShared() {
    printf("Beginning Shared Tests\n");
    auto t1 = std::chrono::high_resolution_clock::now();

    std::shared_ptr<BigCyka> cyka1 = std::make_shared<BigCyka>();
    auto cyka2 = cyka1;
    auto cyka3 = cyka1;

    cyka2.reset();
    cyka3.reset();
    cyka1.reset();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("STD Shared time: %ld\n", d);

    t1 = std::chrono::high_resolution_clock::now();

    std::shared_ptr<BigCyka> mycyka1 = std::make_shared<BigCyka>();
    auto mycyka2 = mycyka1;
    auto mycyka3 = mycyka1;

    mycyka2.reset();
    mycyka3.reset();
    mycyka1.reset();

    t2 = std::chrono::high_resolution_clock::now();
    d = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("My Shared time: %ld\n", d);
}

int main() {
    testUnique();
    printf("\n=== = === = === = === = === = ===\n");
    testShared();

    return 0;
}
