#pragma once
#include "Widgets.h"
#include "../core/Scheduler.h"
#include <string>
#include <vector>
namespace minios::gui {
class SchedulerPanel {
public:
    SchedulerPanel();
    /** Renders and updates the scheduling simulator. */
    void render();
    /** Restores the default dataset and simulation state. */
    void reset();
    /** Serializes editable scheduling state. */ std::string saveState() const;
    /** Restores editable scheduling state. */ bool loadState(const std::string& json);
    /** Returns a short status for the application status bar. */
    const std::string& status() const { return status_; }
private:
    void run(); void loadExample(int index); void exportCsv();
    std::vector<Process> input_; ScheduleResult result_; SchedulingAlgorithm algorithm_{SchedulingAlgorithm::FCFS}; int quantum_{2}; PlaybackState playback_; bool fullTimeline_{false}; std::string error_, status_{"Ready"};
};
}  // namespace minios::gui
