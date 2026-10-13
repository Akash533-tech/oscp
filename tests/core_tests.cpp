#include "core/Scheduler.h"
#include "core/MemoryManager.h"
#include "core/PageReplacement.h"
#include "core/Deadlock.h"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace minios;
int main(){
 auto fifo=PageReplacement::run({7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1},3,PageAlgorithm::FIFO);
 auto lru=PageReplacement::run({7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1},3,PageAlgorithm::LRU);
 auto opt=PageReplacement::run({7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1},3,PageAlgorithm::Optimal);
 assert(fifo.faults==15&&lru.faults==12&&opt.faults==9);
 auto safe=Deadlock::safety({{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}},{{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}},{3,3,2});
 assert(safe.safe&&(safe.sequence==std::vector<int>{1,3,4,0,2}));
 auto fcfs=Scheduler::run({{1,0,5,0},{2,1,3,0},{3,2,1,0}},SchedulingAlgorithm::FCFS);
 assert(std::abs(fcfs.averageWaiting-10.0/3.0)<1e-9);int burst=0;for(auto&s:fcfs.slices)if(s.pid>=0)burst+=s.end-s.start;assert(burst==9);
 auto mem=MemoryManager::allocate({100,500,200,300,600},{212,417,112,426},FitAlgorithm::First);assert((mem.allocation==std::vector<int>{1,4,2,-1}));
 auto belady3=PageReplacement::run({1,2,3,4,1,2,5,1,2,3,4,5},3,PageAlgorithm::FIFO),belady4=PageReplacement::run({1,2,3,4,1,2,5,1,2,3,4,5},4,PageAlgorithm::FIFO);assert(belady3.faults==9&&belady4.faults==10);
 auto cycle=Deadlock::detectCycle(3,{{0,1},{1,2},{2,0}});assert(cycle.hasCycle);
 std::cout<<"All MiniOS core tests passed.\n";
}


