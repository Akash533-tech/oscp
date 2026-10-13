#pragma once
#include "Widgets.h"
#include "../core/MemoryManager.h"
#include <string>
#include <vector>
namespace minios::gui {
class MemoryPanel {
public:
    MemoryPanel();
    /** Renders and updates the memory allocation simulator. */ void render();
    /** Restores defaults. */ void reset();
    /** Serializes editable memory state. */ std::string saveState() const;
    /** Restores editable memory state. */ bool loadState(const std::string& json);
    const std::string& status() const { return status_; }
private:
    void run(); void loadExample(int); void exportCsv();
    std::vector<int> blocks_, processes_; MemoryResult result_; FitAlgorithm algorithm_{FitAlgorithm::First}; PlaybackState playback_; std::string error_, status_{"Ready"}; int freeBlock_{-1};
};
}  // namespace minios::gui
