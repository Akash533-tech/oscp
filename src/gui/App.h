#pragma once
#include "SchedulerPanel.h"
#include "MemoryPanel.h"
#include "PagingPanel.h"
#include "DeadlockPanel.h"
#include <string>
struct GLFWwindow;
namespace minios::gui {
class App {
public:
    /** Creates application state and default datasets. */ App();
    /** Draws a complete application frame. */ void render(GLFWwindow* window);
private:
    void applyTheme(bool dark); void resetAll();
    SchedulerPanel scheduler_; MemoryPanel memory_; PagingPanel paging_; DeadlockPanel deadlock_; bool dark_{true}; int tab_{0}; bool showAbout_{false},showNotes_{false}; std::string lastAction_{"Application started"};
};
}  // namespace minios::gui
