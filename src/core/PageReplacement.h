#pragma once

#include <string>
#include <vector>

namespace minios {
enum class PageAlgorithm { FIFO, LRU, Optimal, Clock };
struct PageStep { int reference{0}; std::vector<int> frames; bool hit{false}; int loadedFrame{-1}; int evicted{-1}; int clockHand{-1}; std::vector<bool> referenceBits; };
struct PageResult { std::vector<PageStep> steps; int faults{0}; int hits{0}; double hitRatio{0}; double faultRatio{0}; };
class PageReplacement {
public:
    /** Simulates the selected page replacement policy. */
    static PageResult run(const std::vector<int>& references, int frameCount, PageAlgorithm algorithm);
    /** Human-readable algorithm name. */
    static const char* name(PageAlgorithm algorithm);
};
}  // namespace minios
