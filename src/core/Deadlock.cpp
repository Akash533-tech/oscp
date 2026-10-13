#include "Deadlock.h"

#include <algorithm>
#include <functional>
#include <stdexcept>

namespace minios {
SafetyResult Deadlock::safety(const std::vector<std::vector<int>>& alloc, const std::vector<std::vector<int>>& max, const std::vector<int>& avail) {
    if (alloc.empty() || avail.empty() || alloc.size() != max.size()) throw std::invalid_argument("Banker matrices cannot be empty and must have matching rows.");
    const int n = static_cast<int>(alloc.size()), m = static_cast<int>(avail.size()); SafetyResult out;
    out.need.assign(n, std::vector<int>(m));
    for (int i = 0; i < n; ++i) {
        if (static_cast<int>(alloc[i].size()) != m || static_cast<int>(max[i].size()) != m) throw std::invalid_argument("All matrix rows must match resource count.");
        for (int j = 0; j < m; ++j) { if (alloc[i][j] < 0 || max[i][j] < alloc[i][j]) throw std::invalid_argument("Require 0 <= Allocation <= Max."); out.need[i][j] = max[i][j] - alloc[i][j]; }
    }
    std::vector<int> work = avail; std::vector<bool> finish(n, false); out.steps.push_back({work, finish, -1, "Initialize Work = Available"});
    int scanStart = 0;
    while (static_cast<int>(out.sequence.size()) < n) {
        int chosen = -1;
        for (int offset = 0; offset < n && chosen < 0; ++offset) {
            const int i = (scanStart + offset) % n;
            if (!finish[i]) {
            bool possible = true; for (int j = 0; j < m; ++j) possible &= out.need[i][j] <= work[j];
            if (possible) chosen = i;
            }
        }
        if (chosen < 0) { out.safe = false; out.explanation = "No unfinished process has Need <= Work."; out.steps.push_back({work, finish, -1, out.explanation}); return out; }
        for (int j = 0; j < m; ++j) work[j] += alloc[chosen][j]; finish[chosen] = true; out.sequence.push_back(chosen);
        scanStart = (chosen + 1) % n;
        out.steps.push_back({work, finish, chosen, "P" + std::to_string(chosen) + " can finish; release its allocation"});
    }
    out.safe = true; out.explanation = "All processes can finish in the displayed sequence."; return out;
}

RequestResult Deadlock::request(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& m, const std::vector<int>& v, int p, const std::vector<int>& req) {
    RequestResult rr; if (p < 0 || p >= static_cast<int>(a.size()) || req.size() != v.size()) { rr.explanation = "Invalid process or request width."; return rr; }
    for (std::size_t j = 0; j < req.size(); ++j) {
        if (req[j] < 0 || req[j] > m[p][j] - a[p][j]) { rr.explanation = "Request exceeds the process need."; return rr; }
        if (req[j] > v[j]) { rr.explanation = "Resources are not currently available; process must wait."; return rr; }
    }
    auto aa = a; auto vv = v; for (std::size_t j = 0; j < req.size(); ++j) { aa[p][j] += req[j]; vv[j] -= req[j]; }
    rr.safety = safety(aa, m, vv); rr.granted = rr.safety.safe; rr.explanation = rr.granted ? "Request granted: tentative state is safe." : "Request denied: tentative state is unsafe."; return rr;
}

CycleResult Deadlock::detectCycle(int n, const std::vector<GraphEdge>& edges) {
    CycleResult out; if (n <= 0) return out; std::vector<std::vector<int>> adj(n);
    for (auto e : edges) if (e.from >= 0 && e.from < n && e.to >= 0 && e.to < n) adj[e.from].push_back(e.to);
    std::vector<int> color(n), parent(n, -1); int start = -1, end = -1;
    std::function<bool(int)> dfs = [&](int u) { color[u] = 1; for (int v : adj[u]) { if (!color[v]) { parent[v] = u; if (dfs(v)) return true; } else if (color[v] == 1) { start = v; end = u; return true; } } color[u] = 2; return false; };
    for (int i = 0; i < n && start < 0; ++i) if (!color[i]) dfs(i);
    if (start >= 0) { out.hasCycle = true; out.cycle.push_back(start); for (int v = end; v != start; v = parent[v]) out.cycle.push_back(v); out.cycle.push_back(start); std::reverse(out.cycle.begin(), out.cycle.end()); out.deadlockedProcesses = out.cycle; std::sort(out.deadlockedProcesses.begin(), out.deadlockedProcesses.end()); out.deadlockedProcesses.erase(std::unique(out.deadlockedProcesses.begin(), out.deadlockedProcesses.end()), out.deadlockedProcesses.end()); }
    return out;
}
}  // namespace minios
