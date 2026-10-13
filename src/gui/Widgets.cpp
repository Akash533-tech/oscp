#include "Widgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include <algorithm>
#include <cmath>

namespace minios::gui {
void PlaybackState::reset() { step = 0; playing = false; lastAdvance = ImGui::GetTime(); }
void PlaybackState::update(std::size_t count, double now) {
    if (!playing || count == 0) return;
    if (now - lastAdvance >= 1.0 / std::max(0.25f, speed)) {
        if (step + 1 < count) ++step; else playing = false;
        lastAdvance = now;
    }
}
unsigned int colorFor(int id, float alpha) {
    static const ImVec4 colors[] = {{0.20f,0.62f,0.86f,1},{0.95f,0.50f,0.25f,1},{0.35f,0.75f,0.45f,1},{0.70f,0.45f,0.85f,1},{0.95f,0.75f,0.20f,1},{0.20f,0.78f,0.75f,1},{0.90f,0.35f,0.50f,1}};
    ImVec4 c = colors[static_cast<unsigned>(std::abs(id)) % (sizeof(colors)/sizeof(colors[0]))]; c.w = alpha; return ImGui::ColorConvertFloat4ToU32(c);
}
void helpMarker(const char* text) { ImGui::SameLine(); ImGui::TextDisabled("(?)"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", text); }
void playbackControls(const char* id, PlaybackState& s, std::size_t count) {
    ImGui::PushID(id); s.update(count, ImGui::GetTime());
    if (ImGui::Button(s.playing ? "Pause" : "Play")) { s.playing = !s.playing; s.lastAdvance = ImGui::GetTime(); }
    ImGui::SameLine(); if (ImGui::Button("Back") && s.step > 0) { --s.step; s.playing = false; }
    ImGui::SameLine(); if (ImGui::Button("Step") && s.step + 1 < count) { ++s.step; s.playing = false; }
    ImGui::SameLine(); if (ImGui::Button("Reset")) s.reset();
    ImGui::SameLine(); ImGui::SetNextItemWidth(130); ImGui::SliderFloat("Speed", &s.speed, 0.25f, 8.0f, "%.2fx");
    ImGui::SameLine(); ImGui::TextDisabled("Step %zu / %zu", count ? s.step + 1 : 0, count);
    ImGui::PopID();
}
void ganttChart(const std::vector<ScheduleSlice>& slices, int visibleTime, float height) {
    ImVec2 pos = ImGui::GetCursorScreenPos(), avail(ImGui::GetContentRegionAvail().x, height); ImGui::InvisibleButton("gantt", avail);
    auto* dl = ImGui::GetWindowDrawList(); dl->AddRectFilled(pos, pos + avail, IM_COL32(30,34,42,255), 5.0f);
    if (slices.empty()) return; int total = std::max(1, slices.back().end); float xscale = (avail.x - 20) / total; float top = pos.y + 22, bottom = pos.y + 70;
    for (const auto& s : slices) {
        if (s.start >= visibleTime) break; int end = std::min(s.end, visibleTime); if (end <= s.start) continue;
        ImVec2 a(pos.x + 10 + s.start*xscale, top), b(pos.x + 10 + end*xscale, bottom);
        dl->AddRectFilled(a,b,s.pid < 0 ? IM_COL32(90,90,95,255) : colorFor(s.pid),3); dl->AddRect(a,b,IM_COL32(220,220,220,160));
        std::string label = s.pid < 0 ? "Idle" : "P" + std::to_string(s.pid); ImVec2 ts=ImGui::CalcTextSize(label.c_str()); if (b.x-a.x > ts.x+5) dl->AddText(ImVec2((a.x+b.x-ts.x)/2,top+14),IM_COL32_WHITE,label.c_str());
    }
    for (int t=0;t<=total;++t) { float x=pos.x+10+t*xscale; dl->AddLine({x,bottom},{x,bottom+5},IM_COL32(180,180,180,255)); char b[16]; snprintf(b,sizeof(b),"%d",t); dl->AddText({x-3,bottom+8},IM_COL32(190,190,190,255),b); }
}
void barChart(const std::vector<std::string>& labels, const std::vector<float>& values, const char* overlay, float height) {
    ImVec2 p=ImGui::GetCursorScreenPos(), size(ImGui::GetContentRegionAvail().x,height); ImGui::InvisibleButton(overlay,size); auto* dl=ImGui::GetWindowDrawList(); dl->AddRectFilled(p,p+size,IM_COL32(30,34,42,255),4);
    if(values.empty()) return; float max=*std::max_element(values.begin(),values.end()); max=std::max(1.0f,max); float w=(size.x-20)/values.size();
    for(std::size_t i=0;i<values.size();++i){float h=(height-45)*values[i]/max; ImVec2 a(p.x+10+i*w+5,p.y+height-25-h),b(p.x+10+(i+1)*w-5,p.y+height-25); dl->AddRectFilled(a,b,colorFor(static_cast<int>(i)),3); char v[24]; snprintf(v,sizeof(v),"%.1f",values[i]); dl->AddText({a.x,a.y-18},IM_COL32_WHITE,v); dl->AddText({a.x,b.y+5},IM_COL32(210,210,210,255),labels[i].c_str());}
}
}  // namespace minios::gui
