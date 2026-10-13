#include "DiskPanel.h"

#include <imgui.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace minios::gui {
namespace {
std::vector<int> numbers(const std::string& text) {
    std::string copy = text; for (char& c : copy) if (c == ',' || c == '[' || c == ']') c = ' ';
    std::istringstream input(copy); std::vector<int> values; int value; while (input >> value) values.push_back(value); return values;
}
int jsonInt(const std::string& json, const std::string& key, int fallback) {
    const auto pos = json.find('"' + key + '"'); if (pos == std::string::npos) return fallback;
    const auto colon = json.find(':', pos); if (colon == std::string::npos) return fallback;
    try { return std::stoi(json.substr(colon + 1)); } catch (...) { return fallback; }
}
}  // namespace
DiskPanel::DiskPanel(){reset();}
void DiskPanel::loadExample(int i){const char*s=i==0?"98,183,37,122,14,124,65,67":i==1?"82,170,43,140,24,16,190":"10,20,30,160,170,180";std::snprintf(text_.data(),text_.size(),"%s",s);head_=i==0?53:(i==1?50:100);cylinders_=200;run();}
void DiskPanel::reset(){algorithm_=DiskAlgorithm::FCFS;movingRight_=true;loadExample(0);status_="Loaded classic disk example";}
bool DiskPanel::parse(){requests_=numbers(text_.data());if(requests_.empty()){error_="Enter comma-separated cylinder requests.";return false;}return true;}
void DiskPanel::run(){if(!parse())return;try{result_=DiskScheduler::run(requests_,head_,cylinders_,algorithm_,movingRight_);error_.clear();playback_.reset();status_=std::string("Ran ")+DiskScheduler::name(algorithm_);}catch(const std::exception&e){error_=e.what();status_="Input error";}}
void DiskPanel::exportCsv(){std::ofstream f("disk_results.csv");f<<"Step,From,To,Distance,Cumulative,Type\n";for(size_t i=0;i<result_.steps.size();++i){const auto&s=result_.steps[i];f<<i<<','<<s.from<<','<<s.to<<','<<s.distance<<','<<s.cumulative<<','<<(s.boundaryMove?"Boundary/wrap":"Request")<<'\n';}status_=f?"Saved disk_results.csv":"Export failed";}
std::string DiskPanel::saveState()const{std::ostringstream o;o<<"{\"requests\":[";for(size_t i=0;i<requests_.size();++i){if(i)o<<',';o<<requests_[i];}o<<"],\"head\":"<<head_<<",\"cylinders\":"<<cylinders_<<",\"algorithm\":"<<(int)algorithm_<<",\"right\":"<<(movingRight_?1:0)<<'}';return o.str();}
bool DiskPanel::loadState(const std::string& json){auto start=json.find("\"requests\"");if(start==std::string::npos)return false;auto l=json.find('[',start),r=json.find(']',l);if(l==std::string::npos||r==std::string::npos)return false;requests_=numbers(json.substr(l,r-l+1));std::string joined;for(size_t i=0;i<requests_.size();++i){if(i)joined+=',';joined+=std::to_string(requests_[i]);}std::snprintf(text_.data(),text_.size(),"%s",joined.c_str());head_=jsonInt(json,"head",head_);cylinders_=jsonInt(json,"cylinders",cylinders_);algorithm_=(DiskAlgorithm)std::clamp(jsonInt(json,"algorithm",0),0,5);movingRight_=jsonInt(json,"right",1)!=0;run();return error_.empty();}
void DiskPanel::render(){const char*names[]={"FCFS","SSTF","SCAN","C-SCAN","LOOK","C-LOOK"};int a=(int)algorithm_;ImGui::BeginChild("disk_left",{345,0},true);ImGui::TextUnformatted("Cylinder request queue");ImGui::InputTextMultiline("##diskrefs",text_.data(),text_.size(),{-1,72});ImGui::SetNextItemWidth(180);if(ImGui::Combo("Algorithm",&a,names,6)){algorithm_=(DiskAlgorithm)a;run();}ImGui::InputInt("Initial head",&head_);ImGui::InputInt("Cylinder count",&cylinders_);if(a>=2)ImGui::Checkbox("Initially moving right",&movingRight_);static int ex=0;const char*examples[]={"Classic queue","Wide queue","Two clusters"};if(ImGui::Combo("Example",&ex,examples,3))loadExample(ex);if(ImGui::Button("Run"))run();ImGui::SameLine();if(ImGui::Button("Save Results"))exportCsv();if(!error_.empty())ImGui::TextColored({1,.3f,.3f,1},"%s",error_.c_str());ImGui::Separator();ImGui::TextUnformatted("Total movement comparison");std::vector<std::string>labels;std::vector<float>values;for(int i=0;i<6;++i)try{auto x=DiskScheduler::run(requests_,head_,cylinders_,(DiskAlgorithm)i,movingRight_);labels.push_back(names[i]);values.push_back((float)x.totalMovement);}catch(...){ }barChart(labels,values,"Head movement",185);ImGui::EndChild();ImGui::SameLine();
 ImGui::BeginChild("disk_main",{0,0},true);ImGui::Text("%s",DiskScheduler::name(algorithm_));playbackControls("disk",playback_,result_.steps.size());const int upto=result_.steps.empty()?-1:(int)std::min(playback_.step,result_.steps.size()-1);ImVec2 p=ImGui::GetCursorScreenPos(),size(ImGui::GetContentRegionAvail().x,330);ImGui::InvisibleButton("diskplot",size);auto*dl=ImGui::GetWindowDrawList();dl->AddRectFilled(p,{p.x+size.x,p.y+size.y},IM_COL32(28,32,40,255),4);const float left=p.x+35,right=p.x+size.x-20,top=p.y+25,row=std::max(22.0f,(size.y-55)/std::max<size_t>(1,result_.steps.size()+1));dl->AddLine({left,top},{right,top},IM_COL32(180,180,180,255));for(int c=0;c<=4;++c){float x=left+(right-left)*c/4;dl->AddLine({x,top-4},{x,top+4},IM_COL32_WHITE);std::string label=std::to_string((cylinders_-1)*c/4);dl->AddText({x-8,top-21},IM_COL32(210,210,210,255),label.c_str());}auto xpos=[&](int cylinder){return left+(right-left)*cylinder/std::max(1,cylinders_-1);};ImVec2 prev{xpos(head_),top};dl->AddCircleFilled(prev,5,colorFor(0));for(int i=0;i<=upto;++i){const auto&s=result_.steps[i];ImVec2 next{xpos(s.to),top+(i+1)*row};ImU32 col=s.boundaryMove?IM_COL32(245,180,50,255):colorFor(i+1);dl->AddLine(prev,next,col,3);dl->AddCircleFilled(next,5,col);std::string label=std::to_string(s.to);dl->AddText({next.x+7,next.y-7},IM_COL32_WHITE,label.c_str());prev=next;}if(upto>=0){const auto&s=result_.steps[upto];ImGui::SetCursorScreenPos({p.x+12,p.y+size.y-24});ImGui::Text("%d -> %d  distance %d  cumulative %d",s.from,s.to,s.distance,s.cumulative);}ImGui::SetCursorScreenPos({p.x,p.y+size.y+8});ImGui::Text("Service order:");ImGui::SameLine();for(int cylinder:result_.order){ImGui::Text("%d",cylinder);ImGui::SameLine();}ImGui::NewLine();ImGui::Text("Total head movement: %d | Average per request: %.2f",result_.totalMovement,result_.averageMovement);ImGui::EndChild();}
}  // namespace minios::gui
