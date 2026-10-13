# Mini Operating System Simulator

MiniOS is a C++17 desktop application that teaches operating-system algorithms through editable datasets and timer-driven visual playback. The pure `src/core` library owns every algorithm and returns snapshots; `src/gui` only edits inputs, controls playback, draws those snapshots with Dear ImGui, and exports results.

## Features

- CPU scheduling: FCFS, SJF, SRTF, non-preemptive/preemptive priority, and Round Robin; Gantt chart, queue, metrics, and comparison chart.
- Memory allocation: First, Best, Worst, and Next Fit; examined-block playback, fragmentation, utilization, deallocation/compaction demonstration, and comparisons.
- Page replacement: FIFO, LRU, Optimal, and Clock; frame grid, hit/fault colors, Clock hand, Belady demo, comparisons, and faults-vs-frames graph.
- Deadlocks: Banker's safety and resource-request checks, plus a draggable resource-allocation graph with cycle highlighting.
- Disk scheduling: FCFS, SSTF, SCAN, C-SCAN, LOOK, and C-LOOK with animated head movement.
- Three built-in datasets per module, dark/light themes, validation, and CSV export.
- Save and restore the complete simulator configuration through `minios_project.json`.

## Build

CMake downloads GLFW and Dear ImGui automatically during the first configure. A C++17 compiler, Git, CMake 3.16+, and OpenGL development files are required.

### Linux

Install the GLFW build dependencies first (Debian/Ubuntu example):

```bash
sudo apt install build-essential cmake git libgl1-mesa-dev xorg-dev
cmake -S . -B build
cmake --build build -j
./build/MiniOS
```

### macOS

Install CMake with Homebrew or the official installer:

```bash
brew install cmake
cmake -S . -B build
cmake --build build -j
./build/MiniOS
```

### Windows (Visual Studio)

From a Developer PowerShell:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\Release\MiniOS.exe
```

MinGW users can select `-G "MinGW Makefiles"` and run `build\MiniOS.exe`.

## Tests

```bash
cmake --build build --target core_tests
ctest --test-dir build --output-on-failure
```

The tests are deliberately GUI-free. They cover the published page-fault checks, classic Banker's sequence, FCFS timing invariants, First Fit, Belady's anomaly, and graph cycle detection.

## Using the application

Each module is a top-level tab. Edit the inputs or load an example, choose an algorithm, and press **Run**. Use **Play**, **Pause**, **Back**, **Step**, **Reset**, and the speed slider to inspect snapshots. Hover the visualizations for context where available. **Save Results** writes a CSV in the current working directory. The File menu saves or loads every module's editable state as `minios_project.json`.

- In **CPU Scheduling**, priority `0` is higher than priority `1`. The comparison chart displays average waiting time.
- Enable **Show full timeline** to display all Gantt blocks while retaining the current playback step.
- In **Memory Allocation**, yellow overlays show internal fragmentation. Deallocate changes the displayed assignment; Compact combines free capacity and reruns allocation.
- In **Page Replacement**, colored columns distinguish hits and faults. Load the Belady dataset to compare FIFO with three and four frames.
- In **Deadlocks**, inspect `Work`, `Finish`, and `Need` at every safety step. Graph node indices are shown above the canvas; circles are processes, squares are resources, and nodes are draggable.
- In **Disk Scheduling**, choose the initial direction for elevator algorithms and follow each head movement down the chart.

## Screenshots

_Add screenshots of the four application tabs here._

## Verification plan and expected outputs

### CPU scheduling

1. FCFS for `P1(AT=0,BT=5), P2(1,3), P3(2,1)` produces completion times `5, 8, 9` and average waiting time `3.3333`.
2. Round Robin for `P1(0,5), P2(1,3)` with quantum 2 produces completion times `8, 7`; the non-idle Gantt duration is 8.

### Memory allocation

1. First Fit with blocks `100,500,200,300,600` and processes `212,417,112,426` maps processes to blocks `1,4,2,unallocated`.
2. Best Fit on the same input maps them to `3,1,2,4` and reports 433 total internal fragmentation.

### Page replacement

1. For `7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1` with 3 frames: FIFO = 15, LRU = 12, Optimal = 9 faults.
2. FIFO on `1,2,3,4,1,2,5,1,2,3,4,5` has 9 faults with 3 frames and 10 with 4, demonstrating Belady's anomaly.

### Deadlocks

1. The classic 5-process, 3-resource example has safe sequence `P1, P3, P4, P0, P2`.
2. Edges `P0→P1, P1→P2, P2→P0` produce a cycle containing all three processes; removing the final edge produces no cycle.

### Disk scheduling

1. FCFS for `98,183,37,122,14,124,65,67`, starting at 53 on a 200-cylinder disk, moves 640 cylinders.
2. SSTF for the same workload moves 236 cylinders.

## Possible enhancements

- Add multilevel feedback queues and per-queue quantum controls.
- Model fully dynamic variable-partition allocation with address-level coalescing.
- Add LFU/MFU and working-set page policies.
- Animate deadlock recovery by termination or resource preemption.
- Save and load complete simulation projects as JSON.
