#include "prefetcher.h"

std::vector<uint64_t> NextLinePrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    std::vector<uint64_t> prefetches;
    // TODO: Task 3
    // 1. Align current_addr down to the current cache block.
    // 2. Prefetch the next sequential block.

    const uint64_t block_addr = current_addr & ~(block_size - 1); // Align down to block boundary
    prefetches.push_back(block_addr + block_size); // Prefetch the next block
    prefetches.push_back(block_addr + (2 * block_size)); // Keep one more line in flight

    return prefetches;
}

std::vector<uint64_t> StridePrefetcher::calculatePrefetch(uint64_t current_addr, bool miss) {
    // TODO: Task 3
    // Suggested design:
    // 1. Track the previous accessed block.
    // 2. Compute the current stride in block units.
    // 3. If the same stride repeats enough times, prefetch the next block at that stride.
    // 4. Update last_block / last_stride / confidence.
    (void)miss;

    std::vector<uint64_t> prefetches;
    const uint64_t block_addr = current_addr & ~(block_size - 1);
    const uint64_t current_block = block_addr / block_size;

    if (!has_last_block) {
        has_last_block = true;
        last_block = current_block;
        return prefetches;
    }

    const int64_t current_stride = static_cast<int64_t>(current_block) - static_cast<int64_t>(last_block);

    if (current_stride == last_stride) {
        confidence++;
    } else {
        last_stride = current_stride;
        confidence = 1;
    }

    if (confidence >= 2 && current_stride != 0) {
        const int64_t next_block = static_cast<int64_t>(current_block) + current_stride;
        const int64_t second_block = next_block + current_stride;
        if (next_block >= 0) {
            prefetches.push_back(static_cast<uint64_t>(next_block) * block_size);
        }
        if (confidence >= 3 && second_block >= 0) {
            prefetches.push_back(static_cast<uint64_t>(second_block) * block_size);
        }
    }

    last_block = current_block;

    return prefetches;
}

Prefetcher* createPrefetcher(std::string name, uint32_t block_size) {
    if (name == "NextLine") return new NextLinePrefetcher(block_size);
    if (name == "Stride") return new StridePrefetcher(block_size);
    return new NoPrefetcher();
}




