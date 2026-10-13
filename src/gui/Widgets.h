#pragma once

#include "../core/Scheduler.h"

#include <cstddef>
#include <string>
#include <vector>

namespace minios::gui {

struct PlaybackState {
    std::size_t step{0}; bool playing{false}; float speed{2.0f}; double lastAdvance{0.0};
    void reset();
    void update(std::size_t count, double now);
};

/** Draws common playback controls and advances state without blocking. */
void playbackControls(const char* id, PlaybackState& state, std::size_t count);
/** Draws a color-keyed scheduling Gantt chart up to a selected time. */
void ganttChart(const std::vector<ScheduleSlice>& slices, int visibleTime, float height = 105.0f);
/** Draws a compact labeled bar chart. */
void barChart(const std::vector<std::string>& labels, const std::vector<float>& values, const char* overlay, float height = 170.0f);
/** Stable color for an integer identity. */
unsigned int colorFor(int id, float alpha = 1.0f);
/** Shows a marker and tooltip when the previous item is hovered. */
void helpMarker(const char* text);
}  // namespace minios::gui
