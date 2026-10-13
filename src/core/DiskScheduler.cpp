#include "DiskScheduler.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace minios {
namespace {
void move(DiskResult& result, int& head, int destination, bool boundary, const std::string& note) {
    if (head == destination && boundary) return;
    const int distance = std::abs(destination - head);
    result.totalMovement += distance;
    result.steps.push_back({head, destination, distance, result.totalMovement, boundary, note});
    head = destination;
    if (!boundary) result.order.push_back(destination);
}
}  // namespace

const char* DiskScheduler::name(DiskAlgorithm algorithm) {
    switch (algorithm) {
        case DiskAlgorithm::FCFS: return "FCFS";
        case DiskAlgorithm::SSTF: return "SSTF";
        case DiskAlgorithm::SCAN: return "SCAN";
        case DiskAlgorithm::CSCAN: return "C-SCAN";
        case DiskAlgorithm::LOOK: return "LOOK";
        case DiskAlgorithm::CLOOK: return "C-LOOK";
    }
    return "Unknown";
}

DiskResult DiskScheduler::run(const std::vector<int>& requests, int initialHead, int cylinders,
                              DiskAlgorithm algorithm, bool movingRight) {
    if (requests.empty()) throw std::invalid_argument("Add at least one disk request.");
    if (cylinders < 2) throw std::invalid_argument("Disk must contain at least two cylinders.");
    if (initialHead < 0 || initialHead >= cylinders) throw std::invalid_argument("Initial head is outside the disk.");
    for (int request : requests) if (request < 0 || request >= cylinders) throw std::invalid_argument("Every request must be inside the disk.");

    DiskResult result;
    int head = initialHead;
    if (algorithm == DiskAlgorithm::FCFS) {
        for (int request : requests) move(result, head, request, false, "Serve next request in arrival order");
    } else if (algorithm == DiskAlgorithm::SSTF) {
        std::vector<int> pending = requests;
        while (!pending.empty()) {
            auto best = std::min_element(pending.begin(), pending.end(), [&](int a, int b) {
                const int da = std::abs(a - head), db = std::abs(b - head);
                return da != db ? da < db : a < b;
            });
            const int request = *best;
            pending.erase(best);
            move(result, head, request, false, "Serve closest pending request");
        }
    } else {
        std::vector<int> left, right;
        for (int request : requests) (request < initialHead ? left : right).push_back(request);
        std::sort(left.begin(), left.end());
        std::sort(right.begin(), right.end());
        auto ascend = [&](const std::vector<int>& values) { for (int value : values) move(result, head, value, false, "Serve request while moving right"); };
        auto descend = [&](const std::vector<int>& values) { for (auto it = values.rbegin(); it != values.rend(); ++it) move(result, head, *it, false, "Serve request while moving left"); };
        const bool scan = algorithm == DiskAlgorithm::SCAN || algorithm == DiskAlgorithm::CSCAN;
        const bool circular = algorithm == DiskAlgorithm::CSCAN || algorithm == DiskAlgorithm::CLOOK;
        if (movingRight) {
            ascend(right);
            if (circular) {
                const int edge = scan ? cylinders - 1 : (right.empty() ? initialHead : right.back());
                if (scan) move(result, head, edge, true, "Continue to upper boundary");
                if (!left.empty()) move(result, head, scan ? 0 : left.front(), true, "Circular wrap");
                ascend(left);
            } else {
                if (scan) move(result, head, cylinders - 1, true, "Reverse at upper boundary");
                descend(left);
            }
        } else {
            descend(left);
            if (circular) {
                if (scan) move(result, head, 0, true, "Continue to lower boundary");
                if (!right.empty()) move(result, head, scan ? cylinders - 1 : right.back(), true, "Circular wrap");
                descend(right);
            } else {
                if (scan) move(result, head, 0, true, "Reverse at lower boundary");
                ascend(right);
            }
        }
    }
    result.averageMovement = requests.empty() ? 0.0 : static_cast<double>(result.totalMovement) / requests.size();
    return result;
}
}  // namespace minios
