#pragma once
#include "Widgets.h"
#include "../core/PageReplacement.h"
#include <array>
#include <string>
#include <vector>
namespace minios::gui {
class PagingPanel {
public:
    PagingPanel();
    /** Renders and updates the page replacement simulator. */ void render();
    /** Restores defaults. */ void reset();
    const std::string& status() const { return status_; }
private:
    void run(); void loadExample(int); bool parse(); void exportCsv();
    std::array<char,512> text_{}; std::vector<int> refs_; PageResult result_; PageAlgorithm algorithm_{PageAlgorithm::FIFO}; int frames_{3}; PlaybackState playback_; std::string error_, status_{"Ready"};
};
}  // namespace minios::gui
