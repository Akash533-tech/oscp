#pragma once

#include <string>

namespace minios {

/** Input and calculated statistics for a schedulable process. */
struct Process {
    int pid{0};
    int arrival{0};
    int burst{1};
    int priority{0};
    int completion{0};
    int turnaround{0};
    int waiting{0};
    int response{0};
};

}  // namespace minios
