#pragma once

#include "Widgets.h"
#include "../core/DiskScheduler.h"

#include <array>
#include <string>
#include <vector>

namespace minios::gui {
class DiskPanel {
public:
    DiskPanel();
    /** Renders the disk scheduling simulator. */ void render();
    /** Restores the default workload. */ void reset();
    /** Serializes the editable disk state as a JSON object. */ std::string saveState() const;
    /** Restores disk state from its saved JSON object. */ bool loadState(const std::string& json);
    const std::string& status() const { return status_; }
private:
    bool parse(); void run(); void loadExample(int index); void exportCsv();
    std::array<char, 512> text_{}; std::vector<int> requests_; int head_{53}, cylinders_{200};
    bool movingRight_{true}; DiskAlgorithm algorithm_{DiskAlgorithm::FCFS}; DiskResult result_;
    PlaybackState playback_; std::string error_, status_{"Ready"};
};
}  // namespace minios::gui
