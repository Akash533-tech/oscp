#pragma once

#include <string>
#include <vector>

namespace minios {

enum class FitAlgorithm { First, Best, Worst, Next };
struct MemoryStep { int processIndex{-1}; int blockIndex{-1}; std::vector<int> examined; std::string note; };
struct MemoryResult {
    std::vector<int> allocation;
    std::vector<int> remaining;
    std::vector<MemoryStep> steps;
    int internalFragmentation{0};
    int externalFragmentation{0};
    double utilization{0};
};

class MemoryManager {
public:
    /** Allocates process sizes into fixed partitions with the selected fit rule. */
    static MemoryResult allocate(const std::vector<int>& blocks, const std::vector<int>& processes, FitAlgorithm algorithm);
    /** Human-readable algorithm name. */
    static const char* name(FitAlgorithm algorithm);
};
}  // namespace minios
