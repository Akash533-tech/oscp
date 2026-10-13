#include "SchedulerPanel.h"
#include <imgui.h>
#include <algorithm>
#include <fstream>
#include <random>

namespace minios::gui {
SchedulerPanel::SchedulerPanel(){reset();}
void SchedulerPanel::loadExample(int i){
    if(i==0) input_={{1,0,5,2},{2,1,3,1},{3,2,8,3},{4,3,6,2}};
    else if(i==1) input_={{1,0,8,2},{2,1,4,1},{3,2,2,3},{4,3,1,2}};
    else input_={{1,0,4,3},{2,5,2,1},{3,6,5,2}};
    run();
}
void SchedulerPanel::reset(){algorithm_=SchedulingAlgorithm::FCFS;quantum_=2;loadExample(0);status_="Loaded scheduling example";}
void SchedulerPanel::run(){try{result_=Scheduler::run(input_,algorithm_,quantum_);error_.clear();playback_.reset();status_=std::string("Ran ")+Scheduler::name(algorithm_);}catch(const std::exception&e){error_=e.what();status_="Input error";}}
void SchedulerPanel::exportCsv(){std::ofstream f("scheduler_results.csv");f<<"PID,Arrival,Burst,Priority,Completion,Turnaround,Waiting,Response\n";for(auto&p:result_.processes)f<<p.pid<<','<<p.arrival<<','<<p.burst<<','<<p.priority<<','<<p.completion<<','<<p.turnaround<<','<<p.waiting<<','<<p.response<<'\n';status_=f?"Saved scheduler_results.csv":"Export failed";}
void SchedulerPanel::render(){
    const char* names[]={"FCFS","SJF (NP)","SRTF","Priority (NP)","Priority (P)","Round Robin"}; int a=static_cast<int>(algorithm_);
    ImGui::BeginChild("sched_left",{360,0},true); ImGui::TextUnformatted("Process input"); ImGui::SetNextItemWidth(210); if(ImGui::Combo("Algorithm",&a,names,6)){algorithm_=static_cast<SchedulingAlgorithm>(a);run();}
    if(algorithm_==SchedulingAlgorithm::RoundRobin){ImGui::SetNextItemWidth(100);if(ImGui::InputInt("Quantum",&quantum_))quantum_=std::max(1,quantum_);}
    static int example=0; const char* ex[]={"Mixed workload","Short jobs","Idle gap"}; ImGui::SetNextItemWidth(180); if(ImGui::Combo("Example",&example,ex,3))loadExample(example);
    if(ImGui::BeginTable("processes",5,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg)){for(const char*h:{"PID","Arrival","Burst","Priority",""})ImGui::TableSetupColumn(h);ImGui::TableHeadersRow();int remove=-1;for(int i=0;i<(int)input_.size();++i){ImGui::PushID(i);ImGui::TableNextRow();int* vals[]={&input_[i].pid,&input_[i].arrival,&input_[i].burst,&input_[i].priority};for(int c=0;c<4;++c){ImGui::TableSetColumnIndex(c);ImGui::SetNextItemWidth(-1);ImGui::InputInt("##v",vals[c],0,0);}ImGui::TableSetColumnIndex(4);if(ImGui::SmallButton("X"))remove=i;ImGui::PopID();}ImGui::EndTable();if(remove>=0)input_.erase(input_.begin()+remove);}
    if(ImGui::Button("Add")){int id=1;for(auto&p:input_)id=std::max(id,p.pid+1);input_.push_back({id,0,1,0});}ImGui::SameLine();if(ImGui::Button("Randomize")){std::mt19937 g(std::random_device{}());input_.clear();for(int i=0;i<5;++i)input_.push_back({i+1,i?int(g()%6):0,1+int(g()%9),int(g()%5)});}ImGui::SameLine();if(ImGui::Button("Clear"))input_.clear();
    if(ImGui::Button("Run simulation"))run();ImGui::SameLine();if(ImGui::Button("Save Results"))exportCsv(); if(!error_.empty())ImGui::TextColored({1,.3f,.3f,1},"%s",error_.c_str());
    ImGui::Separator();ImGui::TextUnformatted("Compare algorithms");std::vector<std::string> labels;std::vector<float> vals;for(int i=0;i<6;++i)try{auto c=Scheduler::run(input_,static_cast<SchedulingAlgorithm>(i),quantum_);labels.push_back(i==5?"RR":names[i]);vals.push_back((float)c.averageWaiting);}catch(...){ }barChart(labels,vals,"Average waiting time",160);ImGui::EndChild();ImGui::SameLine();
    ImGui::BeginChild("sched_main",{0,0},true);ImGui::Text("%s",Scheduler::name(algorithm_));playbackControls("sched",playback_,result_.steps.size());int visible=0;if(!result_.steps.empty())visible=result_.steps[std::min(playback_.step,result_.steps.size()-1)].time+1;ganttChart(result_.slices,visible);
    if(!result_.steps.empty()){auto&s=result_.steps[std::min(playback_.step,result_.steps.size()-1)];ImGui::Text("Time %d   Running: %s",s.time,s.runningPid<0?"Idle":("P"+std::to_string(s.runningPid)).c_str());ImGui::SameLine();ImGui::TextDisabled("Ready queue:");ImGui::SameLine();for(int p:s.readyQueue){ImGui::Text("P%d",p);ImGui::SameLine();}ImGui::NewLine();}
    if(ImGui::BeginTable("metrics",8,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|ImGuiTableFlags_ScrollY,{0,190})){for(const char*h:{"PID","AT","BT","Priority","CT","TAT","WT","RT"})ImGui::TableSetupColumn(h);ImGui::TableHeadersRow();for(auto&p:result_.processes){ImGui::TableNextRow();int v[]={p.pid,p.arrival,p.burst,p.priority,p.completion,p.turnaround,p.waiting,p.response};for(int c=0;c<8;++c){ImGui::TableSetColumnIndex(c);ImGui::Text("%d",v[c]);}}ImGui::EndTable();}
    ImGui::Text("Averages: Waiting %.2f | Turnaround %.2f | Response %.2f",result_.averageWaiting,result_.averageTurnaround,result_.averageResponse);ImGui::Text("CPU utilization %.2f%% | Throughput %.3f processes/unit",result_.cpuUtilization,result_.throughput);ImGui::EndChild();
}
} // namespace minios::gui
