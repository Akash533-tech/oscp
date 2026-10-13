#pragma once
#include "Widgets.h"
#include "../core/Deadlock.h"
#include <string>
#include <vector>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
namespace minios::gui {
class DeadlockPanel {
public:
    DeadlockPanel();
    /** Renders Banker's algorithm and graph detection tools. */ void render();
    /** Restores textbook defaults. */ void reset();
    /** Serializes editable Banker state. */ std::string saveState() const;
    /** Restores editable Banker state. */ bool loadState(const std::string& json);
    const std::string& status() const { return status_; }
private:
    void loadBanker(int); void runSafety(); void runRequest(); void exportResults(); void loadGraph(bool deadlocked); void drawGraph();
    int n_{5},m_{3}; std::vector<std::vector<int>> alloc_,max_; std::vector<int> avail_,request_; int requestPid_{1}; SafetyResult safety_; RequestResult requestResult_; PlaybackState playback_; std::string error_, status_{"Ready"};
    int graphProcesses_{4}, graphResources_{2}, edgeFrom_{0},edgeTo_{1}; std::vector<GraphEdge> edges_; std::vector<ImVec2> positions_; CycleResult cycle_;
};
}  // namespace minios::gui
