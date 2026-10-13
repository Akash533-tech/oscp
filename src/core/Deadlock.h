#pragma once

#include <string>
#include <vector>

namespace minios {
struct BankerStep { std::vector<int> work; std::vector<bool> finish; int chosen{-1}; std::string note; };
struct SafetyResult { bool safe{false}; std::vector<int> sequence; std::vector<std::vector<int>> need; std::vector<BankerStep> steps; std::string explanation; };
struct RequestResult { bool granted{false}; SafetyResult safety; std::string explanation; };
struct GraphEdge { int from{0}; int to{0}; };
struct CycleResult { bool hasCycle{false}; std::vector<int> cycle; std::vector<int> deadlockedProcesses; };
class Deadlock {
public:
    /** Runs Banker's safety algorithm. */
    static SafetyResult safety(const std::vector<std::vector<int>>& allocation, const std::vector<std::vector<int>>& maximum, const std::vector<int>& available);
    /** Tentatively evaluates one resource request. */
    static RequestResult request(const std::vector<std::vector<int>>& allocation, const std::vector<std::vector<int>>& maximum, const std::vector<int>& available, int process, const std::vector<int>& request);
    /** Finds one directed cycle in a wait-for graph. */
    static CycleResult detectCycle(int processCount, const std::vector<GraphEdge>& edges);
};
}  // namespace minios
