#pragma once

#include "Process.h"

#include <string>
#include <vector>

namespace minios {

enum class SchedulingAlgorithm { FCFS, SJF, SRTF, PriorityNP, PriorityP, RoundRobin };

struct ScheduleSlice { int pid{-1}; int start{0}; int end{0}; };
struct ScheduleStep { int time{0}; int runningPid{-1}; std::vector<int> readyQueue; std::string note; };
struct ScheduleResult {
    std::vector<Process> processes;
    std::vector<ScheduleSlice> slices;
    std::vector<ScheduleStep> steps;
    double averageWaiting{0};
    double averageTurnaround{0};
    double averageResponse{0};
    double cpuUtilization{0};
    double throughput{0};
};

class Scheduler {
public:
    /** Runs a scheduling algorithm and returns its complete playback timeline. */
    static ScheduleResult run(const std::vector<Process>& input, SchedulingAlgorithm algorithm, int quantum = 2);
    /** Human-readable algorithm name. */
    static const char* name(SchedulingAlgorithm algorithm);
};

}  // namespace minios
