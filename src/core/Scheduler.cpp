#include "Scheduler.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace minios {
namespace {
bool arrivedUnfinished(const Process& p, int remaining, int time) { return p.arrival <= time && remaining > 0; }

void addSlice(std::vector<ScheduleSlice>& slices, int pid, int time) {
    if (!slices.empty() && slices.back().pid == pid && slices.back().end == time) ++slices.back().end;
    else slices.push_back({pid, time, time + 1});
}

void finishResult(ScheduleResult& out, const std::vector<int>& first, int busy) {
    if (out.processes.empty()) return;
    int finish = 0;
    for (std::size_t i = 0; i < out.processes.size(); ++i) {
        auto& p = out.processes[i];
        p.turnaround = p.completion - p.arrival;
        p.waiting = p.turnaround - p.burst;
        p.response = first[i] - p.arrival;
        out.averageWaiting += p.waiting;
        out.averageTurnaround += p.turnaround;
        out.averageResponse += p.response;
        finish = std::max(finish, p.completion);
    }
    const double n = static_cast<double>(out.processes.size());
    out.averageWaiting /= n; out.averageTurnaround /= n; out.averageResponse /= n;
    out.cpuUtilization = finish ? 100.0 * busy / finish : 0.0;
    out.throughput = finish ? n / finish : 0.0;
}
}  // namespace

const char* Scheduler::name(SchedulingAlgorithm a) {
    switch (a) {
        case SchedulingAlgorithm::FCFS: return "FCFS";
        case SchedulingAlgorithm::SJF: return "SJF (non-preemptive)";
        case SchedulingAlgorithm::SRTF: return "SRTF";
        case SchedulingAlgorithm::PriorityNP: return "Priority (non-preemptive)";
        case SchedulingAlgorithm::PriorityP: return "Priority (preemptive)";
        case SchedulingAlgorithm::RoundRobin: return "Round Robin";
    }
    return "Unknown";
}

ScheduleResult Scheduler::run(const std::vector<Process>& input, SchedulingAlgorithm algorithm, int quantum) {
    if (input.empty()) throw std::invalid_argument("Add at least one process.");
    if (quantum <= 0) throw std::invalid_argument("Time quantum must be positive.");
    ScheduleResult out; out.processes = input;
    const int n = static_cast<int>(input.size());
    std::vector<int> rem(n), first(n, -1);
    for (int i = 0; i < n; ++i) {
        if (input[i].arrival < 0 || input[i].burst <= 0) throw std::invalid_argument("Arrival must be non-negative and burst must be positive.");
        rem[i] = input[i].burst;
    }
    int done = 0, time = 0, busy = 0, current = -1;

    if (algorithm == SchedulingAlgorithm::RoundRobin) {
        std::deque<int> queue; std::vector<bool> queued(n, false); int used = 0;
        while (done < n) {
            for (int i = 0; i < n; ++i) if (!queued[i] && rem[i] > 0 && input[i].arrival <= time) { queue.push_back(i); queued[i] = true; }
            if (current < 0 && !queue.empty()) { current = queue.front(); queue.pop_front(); used = 0; }
            std::vector<int> ready; for (int i : queue) ready.push_back(input[i].pid);
            out.steps.push_back({time, current < 0 ? -1 : input[current].pid, ready, current < 0 ? "CPU idle" : "Run one quantum tick"});
            addSlice(out.slices, current < 0 ? -1 : input[current].pid, time);
            if (current >= 0) {
                if (first[current] < 0) first[current] = time;
                --rem[current]; ++used; ++busy;
                for (int i = 0; i < n; ++i) if (!queued[i] && i != current && rem[i] > 0 && input[i].arrival == time + 1) { queue.push_back(i); queued[i] = true; }
                if (rem[current] == 0) { out.processes[current].completion = time + 1; ++done; current = -1; }
                else if (used == quantum) { queue.push_back(current); current = -1; }
            }
            ++time;
        }
    } else {
        while (done < n) {
            auto better = [&](int a, int b) {
                if (b < 0) return true;
                if (algorithm == SchedulingAlgorithm::FCFS) return input[a].arrival != input[b].arrival ? input[a].arrival < input[b].arrival : input[a].pid < input[b].pid;
                if (algorithm == SchedulingAlgorithm::SJF || algorithm == SchedulingAlgorithm::SRTF) {
                    const int av = algorithm == SchedulingAlgorithm::SRTF ? rem[a] : input[a].burst;
                    const int bv = algorithm == SchedulingAlgorithm::SRTF ? rem[b] : input[b].burst;
                    return av != bv ? av < bv : (input[a].arrival != input[b].arrival ? input[a].arrival < input[b].arrival : input[a].pid < input[b].pid);
                }
                return input[a].priority != input[b].priority ? input[a].priority < input[b].priority : input[a].pid < input[b].pid;
            };
            const bool preemptive = algorithm == SchedulingAlgorithm::SRTF || algorithm == SchedulingAlgorithm::PriorityP;
            if (current < 0 || preemptive) {
                int chosen = -1;
                for (int i = 0; i < n; ++i) if (arrivedUnfinished(input[i], rem[i], time) && better(i, chosen)) chosen = i;
                current = chosen;
            }
            std::vector<int> ready;
            for (int i = 0; i < n; ++i) if (i != current && arrivedUnfinished(input[i], rem[i], time)) ready.push_back(input[i].pid);
            out.steps.push_back({time, current < 0 ? -1 : input[current].pid, ready, current < 0 ? "CPU idle" : "Execute one tick"});
            addSlice(out.slices, current < 0 ? -1 : input[current].pid, time);
            if (current >= 0) {
                if (first[current] < 0) first[current] = time;
                --rem[current]; ++busy;
                if (rem[current] == 0) { out.processes[current].completion = time + 1; ++done; current = -1; }
            }
            ++time;
        }
    }
    finishResult(out, first, busy);
    return out;
}
}  // namespace minios
