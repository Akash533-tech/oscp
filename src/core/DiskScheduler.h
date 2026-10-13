#pragma once

#include <string>
#include <vector>

namespace minios {

enum class DiskAlgorithm { FCFS, SSTF, SCAN, CSCAN, LOOK, CLOOK };

struct DiskStep {
    int from{0};
    int to{0};
    int distance{0};
    int cumulative{0};
    bool boundaryMove{false};
    std::string note;
};

struct DiskResult {
    std::vector<int> order;
    std::vector<DiskStep> steps;
    int totalMovement{0};
    double averageMovement{0.0};
};

class DiskScheduler {
public:
    /** Simulates a disk scheduling policy and returns every head movement. */
    static DiskResult run(const std::vector<int>& requests, int initialHead, int cylinders,
                          DiskAlgorithm algorithm, bool movingRight = true);
    /** Returns the display name of a disk scheduling policy. */
    static const char* name(DiskAlgorithm algorithm);
};

}  // namespace minios
