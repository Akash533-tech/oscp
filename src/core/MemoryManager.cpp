#include "MemoryManager.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace minios {
const char* MemoryManager::name(FitAlgorithm a) {
    switch (a) { case FitAlgorithm::First: return "First Fit"; case FitAlgorithm::Best: return "Best Fit"; case FitAlgorithm::Worst: return "Worst Fit"; case FitAlgorithm::Next: return "Next Fit"; }
    return "Unknown";
}

MemoryResult MemoryManager::allocate(const std::vector<int>& blocks, const std::vector<int>& processes, FitAlgorithm algorithm) {
    if (blocks.empty() || processes.empty()) throw std::invalid_argument("Add at least one block and process.");
    for (int v : blocks) if (v <= 0) throw std::invalid_argument("Block sizes must be positive.");
    for (int v : processes) if (v <= 0) throw std::invalid_argument("Process sizes must be positive.");
    MemoryResult r; r.allocation.assign(processes.size(), -1); r.remaining = blocks;
    std::vector<bool> used(blocks.size(), false); std::size_t next = 0; int allocatedBytes = 0;
    for (std::size_t p = 0; p < processes.size(); ++p) {
        int pick = -1; std::vector<int> examined;
        if (algorithm == FitAlgorithm::Next) {
            for (std::size_t k = 0; k < blocks.size(); ++k) {
                int i = static_cast<int>((next + k) % blocks.size()); examined.push_back(i);
                r.steps.push_back({static_cast<int>(p), -1, examined, "Examining block " + std::to_string(i)});
                if (!used[i] && blocks[i] >= processes[p]) { pick = i; next = (i + 1) % blocks.size(); break; }
            }
        } else {
            for (std::size_t i = 0; i < blocks.size(); ++i) {
                examined.push_back(static_cast<int>(i));
                r.steps.push_back({static_cast<int>(p), -1, examined, "Examining block " + std::to_string(i)});
                if (used[i] || blocks[i] < processes[p]) continue;
                if (pick < 0 || (algorithm == FitAlgorithm::Best && blocks[i] < blocks[pick]) || (algorithm == FitAlgorithm::Worst && blocks[i] > blocks[pick])) pick = static_cast<int>(i);
                if (algorithm == FitAlgorithm::First) break;
            }
        }
        if (pick >= 0) {
            used[pick] = true; r.allocation[p] = pick; r.remaining[pick] = blocks[pick] - processes[p];
            r.internalFragmentation += r.remaining[pick]; allocatedBytes += processes[p];
            r.steps.push_back({static_cast<int>(p), pick, examined, "Allocated P" + std::to_string(p) + " to block " + std::to_string(pick)});
        } else r.steps.push_back({static_cast<int>(p), -1, examined, "P" + std::to_string(p) + " could not be allocated"});
    }
    int freeTotal = 0; for (std::size_t i = 0; i < blocks.size(); ++i) if (!used[i]) freeTotal += blocks[i];
    r.externalFragmentation = freeTotal;
    const int total = std::accumulate(blocks.begin(), blocks.end(), 0);
    r.utilization = total ? 100.0 * allocatedBytes / total : 0.0;
    return r;
}
}  // namespace minios
