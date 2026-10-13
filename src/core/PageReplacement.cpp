#include "PageReplacement.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace minios {
const char* PageReplacement::name(PageAlgorithm a) {
    switch (a) { case PageAlgorithm::FIFO: return "FIFO"; case PageAlgorithm::LRU: return "LRU"; case PageAlgorithm::Optimal: return "Optimal"; case PageAlgorithm::Clock: return "Clock"; }
    return "Unknown";
}
PageResult PageReplacement::run(const std::vector<int>& refs, int count, PageAlgorithm alg) {
    if (refs.empty()) throw std::invalid_argument("Reference string cannot be empty.");
    if (count < 1 || count > 10) throw std::invalid_argument("Frames must be between 1 and 10.");
    PageResult r; std::vector<int> frames(count, -1), last(count, -1); std::vector<bool> bits(count, false); int hand = 0;
    for (int t = 0; t < static_cast<int>(refs.size()); ++t) {
        const int page = refs[t]; auto it = std::find(frames.begin(), frames.end(), page);
        if (it != frames.end()) {
            int slot = static_cast<int>(it - frames.begin()); ++r.hits; last[slot] = t; if (alg == PageAlgorithm::Clock) bits[slot] = true;
            r.steps.push_back({page, frames, true, -1, -1, hand, bits}); continue;
        }
        ++r.faults; int slot = -1;
        auto empty = std::find(frames.begin(), frames.end(), -1);
        if (empty != frames.end()) slot = static_cast<int>(empty - frames.begin());
        else if (alg == PageAlgorithm::FIFO) { slot = hand; hand = (hand + 1) % count; }
        else if (alg == PageAlgorithm::LRU) slot = static_cast<int>(std::min_element(last.begin(), last.end()) - last.begin());
        else if (alg == PageAlgorithm::Optimal) {
            int farthest = -1;
            for (int f = 0; f < count; ++f) {
                int next = std::numeric_limits<int>::max();
                for (int k = t + 1; k < static_cast<int>(refs.size()); ++k) if (refs[k] == frames[f]) { next = k; break; }
                if (next > farthest) { farthest = next; slot = f; }
            }
        } else {
            while (bits[hand]) { bits[hand] = false; hand = (hand + 1) % count; }
            slot = hand; hand = (hand + 1) % count;
        }
        int evicted = frames[slot]; frames[slot] = page; last[slot] = t; if (alg == PageAlgorithm::Clock) bits[slot] = true;
        r.steps.push_back({page, frames, false, slot, evicted, hand, bits});
    }
    const double n = static_cast<double>(refs.size()); r.hitRatio = 100.0 * r.hits / n; r.faultRatio = 100.0 * r.faults / n; return r;
}
}  // namespace minios
